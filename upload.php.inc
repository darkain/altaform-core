<?php

require_once('amazon.php.inc');


use Aws\S3\S3Client;
use Aws\S3\Enum\Group;
use Aws\S3\Enum\CannedAcl;
use Aws\Common\Enum\Region;



class fileImporter {


	public static function importPath($path, $name=false) {
		try {
			$image = new Imagick($path);
		} catch (ImagickException $e) {
			return false;
		}
		$image->setFilename($path);
		return self::processImage($image, $name);
	}




	public static function importFile($file, $name='STREAM') {
		try {
			$image = new Imagick();
			$image->readImageFile($file, $name);
		} catch (ImagickException $e) {
			return false;
		}
		return self::processImage($image);
	}




	public static function importBlob($blob, $name='BLOB') {
		try {
			$image = new Imagick();
			$image->readImageBlob($blob, $name);
		} catch (ImagickException $e) {
			return false;
		}
		return self::processImage($image);
	}




	public static function importURL($url) {
		$blob = @file_get_contents($url);
		return (!empty($blob)) ? self::importBlob($blob, $url) : false;
	}




	//NOTE:	12 byte minimum file size pulled from PHP comments:
	//		http://php.net/manual/en/function.exif-imagetype.php
	public static function upload($form=false, $database=true) {
		if (empty($_FILES)  ||  !is_array($_FILES)) {
			return false;
		} else if (empty($form)) {
			$data = reset($_FILES);
		} else if (isset($_FILES[$form])  &&  is_array($_FILES[$form])) {
			$data = $_FILES[$form];
		}

		if ( empty($data))				return false;
		if (!is_array($data))			return false;
		if (!empty($data['error']))		return false;
		if ( empty($data['size']))		return false;
		if ( empty($data['name']))		return false;
		if ( empty($data['tmp_name']))	return false;
		if ($data['size'] < 12)			return false;
		if (!is_uploaded_file($data['tmp_name'])) return false;

		//TODO:	IF $DATABASE=TRUE, CHECK HASH AGAINST DATABASE HERE!
		//		IF ALREADY IN DATABASE, NO NEED TO PROCESS/UPLOAD IMAGE

		$image = self::importPath($data['tmp_name'], $data['name']);

		if ($database) self::database($image);

		return $image;
	}




	public static function importFacebookProfileImage($facebook_id, $database=true) {
		$image = self::importURL(
			'http://graph.facebook.com/v2.4/' . $facebook_id . '/picture?width=9999&height=9999'
		);

		if ($database) self::database($image);

		return $image;
	}




	private static function processImage(&$image, $name=false) {
		//ONLY SUPPORT JPEG / PNG / GIF FILES FOR NOW
		switch ($image->getImageFormat()) {
			case 'JPEG':  case 'PNG':  case 'GIF':  continue;
			default: return false;
		}

		//BUILD DATA TO RETURN
		$data['exif']		= self::getExif($image);
		$data['hash']		= self::getHash($image);
		$data['url']		= afurl::cdn($data['hash']);
		$data['size']		= $image->getImageLength();
		$data['mime']		= $image->getImageMimeType();
		$data['type']		= $image->getImageFormat();
		$data['name']		= basename(!empty($name) ? $name : $image->getFilename());
		$data['file_hash']	= pudlFunction::unhex($data['hash']);

		//UPLOAD ORIGINAL IMAGE TO AMAZON AWS S3
		self::s3upload(
			$image->getImageBlob(),
			$data['hash'],
			$image->getImageMimeType()
		);

		//RORATE THE IMAGE, IF NEEDED
		switch ($image->getImageOrientation()) {
			case Imagick::ORIENTATION_UNDEFINED: break;	//NOT SET
			case Imagick::ORIENTATION_TOPLEFT: break;	//ALREADY CORRECT

			case Imagick::ORIENTATION_TOPRIGHT:
				$image->flopImage();
			break;

			case Imagick::ORIENTATION_BOTTOMRIGHT:
				$image->rotateImage('#fff', 180);
			break;

			case Imagick::ORIENTATION_BOTTOMLEFT:
				$image->flipImage();
			break;

			case Imagick::ORIENTATION_LEFTTOP:
				$image->transposeImage();
			break;

			case Imagick::ORIENTATION_RIGHTTOP:
				$image->rotateImage('#fff', 90);
			break;

			case Imagick::ORIENTATION_RIGHTBOTTOM:
				$image->transverseImage();
			break;

			case Imagick::ORIENTATION_LEFTBOTTOM:
				$image->rotateImage('#fff', -90);
			break;
		}

		//RESET ORIENTATION INFORMATION
		$image->setImageOrientation(Imagick::ORIENTATION_TOPLEFT);

		//GET THE LONG EDGE OF THE IMAGE
		$long = $image->getImageWidth() > $image->getImageHeight() ?
				$image->getImageWidth() : $image->getImageHeight();

		//PROCESS RESIZED IMAGES (MAINTAIN ASPECT RATIO)
		foreach (self::$imageSize as $size) {
			//TODO: force processing of smallest size, in case of rotation
			if ($size >= $long) continue;
			$data[$size] = self::resize($image, $size, false);
		}

		//PROCESS THUMBNAILS (SQUARE CROP)
		foreach (self::$thumbSize as $size) {
			$data[$size] = self::resize($image, $size, true);
		}

		//CLEANUP / FREE RESOURCES
		$image->clear();

		//RETURN THE EVERYTHINGS!
		return $data;
	}




	private static function resize(&$image, $size, $square=false) {
		$resized = clone $image;

		if ($square) {
			$resized->cropThumbnailImage($size, $size);
		} else {
			$resized->resizeImage($size, $size, Imagick::FILTER_SINC, 1, true);
		}

		$resized->setImagePage(0, 0, 0, 0);
		$resized->setImageCompressionQuality(85);

		//GET BINARY DATA
		$blob = $resized->getImageBlob();

		//BUILD OUR DATA ARRAY
		$data['type']		= (string) $size;
		$data['hash']		= md5($blob);
		$data['size']		= strlen($blob);
		$data['url']		= afurl::cdn($data['hash']);
		$data['thumb_hash']	= pudlFunction::unhex($data['hash']);

		//UPLOAD TO AMAZON AWS S3
		self::s3upload($blob, $data['hash'], $resized->getImageMimeType());

		//CLEANUP / FREE RESOURCES
		$resized->clear();

		//RETURN OUR DATA ARRAY!
		return $data;
	}




	private static function getExif(&$image) {
		$path = $image->getFilename();

		//LOCAL FILE, MUCH FASTER TO USE FILE SYSTEM THAN TO GENERATE A BLOB
		if (!empty($path)) {
			if (substr($path, 0, 1) === '/'  ||  substr($path, 0, 7) === 'file://') {
				return self::exif_read_data($path);
			}
		}

		return self::exif_read_data(
			$exif = 'data://' . $image->getImageMimeType() . ';base64,' . base64_encode(
				substr($image->getImageBlob(), 0, 1024*1024)
			)
		);
	}




	private static function getHash(&$image) {
		$path = $image->getFilename();

		//LOCAL FILE, MUCH FASTER TO USE FILE SYSTEM THAN TO GENERATE A BLOB
		if (!empty($path)) {
			if (substr($path, 0, 1) === '/'  ||  substr($path, 0, 7) === 'file://') {
				return md5_file($path);
			}
		}

		return md5($image->getImageBlob());
	}




	public static function exif_read_data($source) {
		$exif = @exif_read_data($source);
		if (empty($exif)) return false;

		//NOTE: PROPIETARY BINARY DATA, JUST REMOVE IT
		unset($exif['MakerNote']);

		$fix = [
			'A430' => 'CameraOwnerName',
			'A431' => 'BodySerialNumber',
			'A432' => 'LensSpecification',
			'A433' => 'LensMake',
			'A434' => 'LensModel',
			'A435' => 'LensSerialNumber',
			'8830' => 'SensitivityType',
			'8832' => 'RecommendedExposureIndex',
		];

		foreach ($fix as $key => $item) {
			if (isset($exif['UndefinedTag:0x'.$key])) {
				$exif[$item] = $exif['UndefinedTag:0x'.$key];
				unset($exif['UndefinedTag:0x'.$key]);
			}
		}

		foreach ($exif as $key => &$item) {
			if (substr($key, 0, 15) === 'UndefinedTag:0x') {
				unset($exif[$key]);
			} else {
				self::exifClean($item);
			}
		} unset($item);

		return $exif;
	}




	public static function exifClean(&$data) {
		if (is_string($data)) {
			$data = trim($data);
			$data = preg_replace('/[\x00-\x1F]/', '', $data);
			$data = mb_convert_encoding($data, 'UTF-8', 'UTF-8');
		} else if (is_array($data)) {
			foreach ($data as &$item) self::exifClean($item);
		}
	}




	public static function s3client() {
		global $site;

		if (empty(self::$s3client)) {
			self::$s3client = S3Client::factory([
				'key'		=> $site['aws_key'],
				'secret'	=> $site['aws_secret'],
				'base_url'	=> $site['aws_url'],
			]);
		}

		return self::$s3client;
	}




	public static function s3upload($body, $hash, $mime) {
		global $site;

		$client = self::s3client();

		//EARLY OUT
		if (empty($body)) return false;

		//UPLOAD TO AMAZON AWS S3
		$result = $client->putObject([
			'Body'			=> $body,
			'Bucket'		=> $site['aws_bucket'],
			'Key'			=> afurl::cdnPath($hash),
			'ContentType'	=> $mime,
			'acl'			=> CannedAcl::PUBLIC_READ,
			'GrantRead'		=> 'uri="' . Group::ALL_USERS . '"',
			'CacheControl'	=> 'max-age=315360000, public',
			'Expires'		=> gmdate('D, d M Y H:i:s T', strtotime('+10 years')),
		]);

		//WE CAN POLL THE OBJECT UNTIL IT IS ACCESSIBLE
		$client->waitUntilObjectExists([
			'Bucket'	=> $site['aws_bucket'],
			'Key'		=> afurl::cdnPath($hash),
		]);

		return $result;
	}



	public static function database($data) {
		global $db, $user;

		if (empty($data)) return;

		//GET INTERNAL MIME TYPE DATA
		$mime = $db->rowId('pudl_mimetype', 'mime_type', $db->safe($data['mime']));

		//INSERT MAIN IMAGE
		$db->insert('pudl_file', [
			'file_hash'		=> pudlFunction::unhex($data['hash']),
			'file_size'		=> $data['size'],
			'file_uploaded'	=> $db->time(),
			'file_name'		=> $data['name'],
			'mime_id'		=> !empty($mime) ? $mime['mime_id'] : NULL,
		], true, 'file_hash=file_hash');

		//INSERT EXIF DATA
		if (!empty($data['exif'])) {
			$db->insert('pudl_file_meta', [
				'file_hash'			=> pudlFunction::unhex($data['hash']),
				'file_meta_name'	=> 'exif',
				'file_meta_value'	=> $data['exif'],
			], true, 'file_hash=file_hash');
		}

		//ASSOCIATE FILE WITH CURRENT USER, IF AVAILABLE
		if (!empty($user['user_id'])) {
			$db->insert('pudl_file_user', [
				'file_hash'		=> pudlFunction::unhex($data['hash']),
				'user_id'		=> $user['user_id'],
				'user_time'		=> $db->time(),
			], true, 'file_hash=file_hash');
		}

		//INSERT THUMBNAIL IMAGES
		foreach ($data as $key => $item) {
			if (((int)$key) < 1  ||  !is_array($item)) continue;

			$db->insert('pudl_file_thumb', [
				'file_hash'		=> pudlFunction::unhex($data['hash']),
				'thumb_hash'	=> pudlFunction::unhex($item['hash']),
				'thumb_size'	=> $item['size'],
				'thumb_type'	=> $item['type'],
			], true, 'thumb_hash=thumb_hash');
		}
	}



	public static $thumbSize	= [50, 100, 150, 200];
	public static $imageSize	= [800, 1280, 1920];
	public static $s3client		= NULL;
}
