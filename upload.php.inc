<?php


class fileImporter {


	public static function importPath($path, $name=false) {
		try {
			$image = new Imagick($path);
		} catch (ImagickException $e) {
			self::$error = 'importPath error: ' . $path;
			return false;
		}
		$image->setFilename($path);
		$return = self::processImage($image, $name);
		$image->clear();
		return $return;
	}




	public static function importFile($file, $name='STREAM', $hash=false) {
		try {
			$image = new Imagick();
			$image->readImageFile($file, $name);
		} catch (ImagickException $e) {
			self::$error = 'importFile error: ' . $name;
			return false;
		}
		$return = self::processImage($image, false, $hash);
		$image->clear();
		return $return;
	}




	public static function importBlob($blob, $name='BLOB') {
		try {
			$image = new Imagick();
			$image->readImageBlob($blob, $name);
		} catch (ImagickException $e) {
			self::$error = 'importBlob error: ' . $name;
			return false;
		}
		$return = self::processImage($image);
		$image->clear();
		return $return;
	}




	public static function importURL($url) {
		$blob = @file_get_contents($url);

		if ($blob !== false) return self::importBlob($blob, $url);

		self::$error = 'importURL error: ' . $url;
		return false;
	}




	//NOTE:	12 byte minimum file size pulled from PHP comments:
	//		http://php.net/manual/en/function.exif-imagetype.php
	public static function upload($form=false, $database=true) {
		if (empty($_FILES)  ||  !tbx_array($_FILES)) {
			self::$error = 'upload error';
			return false;
		} else if (empty($form)) {
			$data = reset($_FILES);
		} else if (isset($_FILES[$form])  &&  tbx_array($_FILES[$form])) {
			$data = $_FILES[$form];
		}

		switch (true) {
			case empty($data):
				self::$error = 'error: nothing to process';
			return false;

			case !tbx_array($data):
				self::$error = 'error: not an array';
			return false;

			case !empty($data['error']):
				self::$error = 'external error: ' . $data['error'];
			return false;

			case empty($data['size']):
				self::$error = 'error: invalid file size';
			return false;

			case empty($data['name']):
				self::$error = 'error: invalid file name';
			return false;

			case empty($data['tmp_name']):
				self::$error = 'error: invalid temp file';
			return false;

			case $data['size'] < 12:
				self::$error = 'error: file size too small';
			return false;

			case !is_uploaded_file($data['tmp_name']):
				self::$error = 'error: possible hacking attempt';
			return false;
		}

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




	private static function processImage(&$image, $name=false, $hash=false) {
		global $af, $afurl;

		//ONLY SUPPORT JPEG / PNG / GIF FILES FOR NOW
		try {
			switch (strtoupper($image->getImageFormat())) {
				case 'JPEG':  case 'JPG':  case 'PNG':  case 'GIF':
				continue;

				default:
					self::$error = 'invalid file format';
				return false;
			}
		} catch (ImagickException $exception) {
			self::$error = 'cannot read file format';
			return false;
		}

		if (empty($name)) $name = $image->getFilename();

		//BUILD DATA TO RETURN
		$data = [
			'average'		=> self::average($image),
			'exif'			=> self::getExif($image),
			'hash'			=> self::getHash($image, $hash),
			'size'			=> $image->getImageLength(),
			'mime'			=> $image->getImageMimeType(),
			'type'			=> $image->getImageFormat(),
			'name'			=> basename($name),
		];

		$data['url']		= $afurl->cdn($data['hash']);
		$data['file_hash']	= hex2bin($data['hash']);


		//SAVE THE FILE TO OUR STORAGE SYSTEM
		self::write($data['hash'], $image->getImageBlob());


		//RORATE THE IMAGE, IF NEEDED
		switch ($image->getImageOrientation()) {
			case Imagick::ORIENTATION_UNDEFINED:	break;	//NOT SET
			case Imagick::ORIENTATION_TOPLEFT:		break;	//ALREADY CORRECT

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


		//PULL WIDTH AND HEIGHT AFTER ORIENTATION FIX
		$data += [
			'width'			=> $image->getImageWidth(),
			'height'		=> $image->getImageHeight(),
		];


		//SET'S THE IMAGES COLOR SPACE TO SRGB - ANDROID/IOS COMPATIBLITY
		try {
			$image->profileImage('icc',
				file_get_contents($af->path().'_altaform/srgb.icc')
			);
		} catch (ImagickException $exception) {}

		//GET THE LONG EDGE OF THE IMAGE
		$long = max($image->getImageWidth(), $image->getImageHeight());

		//PROCESS RESIZED IMAGES (MAINTAIN ASPECT RATIO)
		//NOTE: CREATE SMALLEST SIZE REGARDLESS, IN CASE OF ROTATION/ICC ISSUES
		$smallest = reset(self::$imageSize);
		foreach (self::$imageSize as $size) {
			if ($size >= $long  &&  $size !== $smallest) continue;
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




	public static function average(&$image) {
		try {
			//RESIZE
			$resized = clone $image;
			$resized->posterizeImage(200, false);
			$resized->scaleimage(50, 50);

			$colors = [];

			for ($y=0; $y<50; $y++) {
				for ($x=0; $x<50; $x++) {
					$pixel			= $resized->getImagePixelColor($x, $y);
					$list			= $pixel->getColor();
					list($r,$g,$b)	= array_values($list);
					$pixel->clear();

					if ($r <  16  ||  $g <  16  ||  $b <  16)		continue;
					if ($r > 240  &&  $g > 240  &&  $b > 240)		continue;
					if (abs($r - $g) < 10  &&  abs($r - $b) < 10)	continue;

					$rgb = (($r&0xFF)<<16) | (($g&0xFF)<<8) | ($b&0xFF);

					if (empty($colors[$rgb])) $colors[$rgb] = 0;
					$colors[$rgb]++;
				}
			}

			//RETURN MOST POPULAR COLOR
			if (!empty($colors)) {
				return str_pad(dechex(
					(array_keys($colors, max($colors)))[0]
				), 6, '0', STR_PAD_LEFT);
			}

		} catch (ImagickException $e) {}

		return NULL;
	}




	private static function resize(&$image, $size, $square=false) {
		global $afurl;

		$resized = clone $image;

		//RESIZE THE IMAGE
		if ($square) {
			$resized->cropThumbnailImage($size, $size);
		} else {
			$resized->resizeImage($size, $size, Imagick::FILTER_SINC, 1, true);
		}

		//SET NEW IMAGE PROPERTIES
		$resized->setImagePage(0, 0, 0, 0);
		$resized->setImageCompressionQuality(85);

		//GET BINARY DATA
		$blob = $resized->getImageBlob();

		//BUILD OUR DATA ARRAY
		$data['type']		= (string) $size;
		$data['hash']		= md5($blob);
		$data['size']		= strlen($blob);
		$data['url']		= $afurl->cdn($data['hash']);
		$data['thumb_hash']	= pudlFunction::unhex($data['hash']);

		//COPY BLOB TO IMAGE FILE SERVER
		$data['good']		= self::write($data['hash'], $blob);

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
			'data://' . $image->getImageMimeType() .
			';base64,' . base64_encode(
				substr($image->getImageBlob(), 0, 1024*1024)
			)
		);
	}




	private static function getHash(&$image, $hash=false) {
		if (!empty($hash)) {
			if (is_string($hash)) return self::unhex($hash);
			if (tbx_array($hash)) {
				if (!empty($hash['hash']))			return self::unhex($hash['hash']);
				if (!empty($hash['file_hash']))		return self::unhex($hash['file_hash']);
				if (!empty($hash['thumb_hash']))	return self::unhex($hash['thumb_hash']);
			}
		}

		$path = $image->getFilename();

		//LOCAL FILE, MUCH FASTER TO USE FILE SYSTEM THAN TO GENERATE A BLOB
		if (!empty($path)) {
			if (substr($path, 0, 1) === '/'		||
				substr($path, 0, 3) === '../'	||
				substr($path, 0, 7) === 'file://') {
				return md5_file($path);
			}
		}

		return md5($image->getImageBlob());
	}




	public static function unhex($hash) {
		if (strlen($hash) !== 16) return $hash;
		return bin2hex($hash);
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
			if (extension_loaded('mbstring')) {
				$data = @mb_convert_encoding($data, 'UTF-8', 'UTF-8');
			} else {
				$data = @iconv('UTF-8', 'UTF-8//IGNORE', $data);
			}

			$data = preg_replace('/[\x00-\x1F]/', ' ', $data);
			$data = afstring::doubletrim($data);

		} else if (tbx_array($data)) {
			foreach ($data as &$item) self::exifClean($item);
		}
	}




	public static function write($hash, $blob) {
		global $af, $afurl;
		$path = $af->path() . 'files/' . $afurl->cdnPath($hash);
		af_fs::mkdir(substr($path, 0, -32));
		return @file_put_contents($path, $blob);
	}



	public static function database($data, $transaction=false) {
		global $db, $user;

		if (empty($data)) return;

		if ($transaction) $db->begin();

		//GET INTERNAL MIME TYPE DATA
		$mime = $db->rowId('pudl_mimetype', 'mime_type', $data['mime']);

		//INSERT MAIN IMAGE
		$db->insert('pudl_file', [
			'file_hash'				=> pudlFunction::unhex($data['hash']),
			'file_size'				=> $data['size'],
			'file_uploaded'			=> $db->time(),
			'file_name'				=> $data['name'],
			'file_width'			=> $data['width'],
			'file_height'			=> $data['height'],
			'file_average'			=> $data['average'],
			'mime_id'				=> !empty($mime) ? $mime['mime_id'] : NULL,
		], 'file_hash=file_hash');

		//INSERT EXIF DATA
		if (!empty($data['exif'])) {
			$db->insert('pudl_file_meta', [
				'file_hash'			=> pudlFunction::unhex($data['hash']),
				'file_meta_name'	=> 'exif',
				'file_meta_value'	=> $data['exif'],
			], 'file_hash=file_hash');
		}

		//ASSOCIATE FILE WITH CURRENT USER, IF AVAILABLE
		if (!empty($user['user_id'])) {
			$db->insert('pudl_file_user', [
				'file_hash'			=> pudlFunction::unhex($data['hash']),
				'user_id'			=> $user['user_id'],
				'user_time'			=> $db->time(),
			], 'file_hash=file_hash');
		}

		self::databaseThumbs($data);

		if ($transaction) $db->commit();
	}



	//INSERT THUMBNAIL IMAGES
	public static function databaseThumbs($data) {
		global $db;

		if (empty($data)) return;

		foreach ($data as $key => $item) {
			if (((int)$key) < 1  ||  !tbx_array($item)) continue;

			$db->insert('pudl_file_thumb', [
				'file_hash'			=> pudlFunction::unhex($data['hash']),
				'thumb_hash'		=> pudlFunction::unhex($item['hash']),
				'thumb_size'		=> $item['size'],
				'thumb_type'		=> $item['type'],
			], 'thumb_hash=thumb_hash');
		}
	}



	//GET THE LAST ERROR MESSAGE
	public static function error() { return self::$error; }



	public	static $thumbSize		= [50, 100, 150, 200];
	public	static $imageSize		= [800, 1280, 1920];
	private	static $error			= false;
}
