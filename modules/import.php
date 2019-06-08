<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// IMPORT FILES INTO THE CDN AND DATABASE
////////////////////////////////////////////////////////////////////////////////
class import {




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR - NEEDS AN ALTAFORM AND A PUDL INSTANCE
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\altaform $af, \pudl $pudl=NULL) {
		$this->af	= $af;
		$this->pudl	= $pudl;
	}




	////////////////////////////////////////////////////////////////////////////
	// IMPORT A FILE FROM THE LOCAL FILE SYSTEM
	// importPath
	////////////////////////////////////////////////////////////////////////////
	public function file($path, $name=false) {
		$image	= new \Imagick($path);
		$image->setFilename($path);
		$return	= $this->process($image, $name);
		$image->clear();
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// IMPORT AN IMAGE FROM THE GIVEN OPENED STREAM
	// importFile
	////////////////////////////////////////////////////////////////////////////
	public function stream($file, $name='STREAM', $hash=false) {
		$image	= new \Imagick();
		$image->readImageFile($file, $name);
		$return	= $this->process($image, false, $hash);
		$image->clear();
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// IMPORT AN IMAGE FROM RAM
	//importBlob
	////////////////////////////////////////////////////////////////////////////
	public function blob($blob, $name='BLOB') {
		$image = new \Imagick();
		$image->readImageBlob($blob, $name);
		$return = $this->process($image);
		$image->clear();
		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// IMPORT AN IMAGE FROM THE GIVEN EXTERNAL URL
	//importURL
	////////////////////////////////////////////////////////////////////////////
	public function url($url) {
		$blob = @file_get_contents($url);

		if (empty($blob)){
			throw new \af\exception\import(__METHOD__ . ' error: ' . $url);
		}

		return $this->blob($blob, $url);
	}




	////////////////////////////////////////////////////////////////////////////
	// HANDLE UPLOADED FILE
	//	NOTE:	12 byte minimum file size pulled from PHP comments:
	//			http://php.net/manual/en/function.exif-imagetype.php
	////////////////////////////////////////////////////////////////////////////
	public function upload($form=false, $database=true) {
		if (empty($_FILES)  ||  !tbx_array($_FILES)) {
			throw new \af\exception\import(__METHOD__ . ' error');

		} else if (empty($form)) {
			$data = reset($_FILES);

		} else if (isset($_FILES[$form])  &&  tbx_array($_FILES[$form])) {
			$data = $_FILES[$form];
		}

		switch (true) {
			case empty($data):
				throw new \af\exception\import(__METHOD__ . ' error: nothing to process');

			case !tbx_array($data):
				throw new \af\exception\import(__METHOD__ . ' error: not an array');

			case !empty($data['error']):
				throw new \af\exception\import(__METHOD__ . ' external error: ' . $data['error']);

			case empty($data['size']):
				throw new \af\exception\import(__METHOD__ . ' error: invalid file size');

			case empty($data['name']):
				throw new \af\exception\import(__METHOD__ . ' error: invalid file name');

			case empty($data['tmp_name']):
				throw new \af\exception\import(__METHOD__ . ' error: invalid temp file');

			case $data['size'] < 12:
				throw new \af\exception\import(__METHOD__ . ' error: file size too small');

			case !is_uploaded_file($data['tmp_name']):
				throw new \af\exception\import(__METHOD__ . ' error: possible hacking attempt');
		}

		//TODO:	IF $DATABASE=TRUE, CHECK HASH AGAINST DATABASE HERE!
		//		IF ALREADY IN DATABASE, NO NEED TO PROCESS/UPLOAD IMAGE

		$image = $this->file($data['tmp_name'], $data['name']);

		if ($database) $this->database($image);

		return $image;
	}




	////////////////////////////////////////////////////////////////////////////
	//
	////////////////////////////////////////////////////////////////////////////
	public function facebook($facebook_id, $database=true) {
		$image = $this->importURL(implode('/', [
			'http://graph.facebook.com/v3.3',
			$facebook_id,
			'picture?width=9999&height=9999',
		]));

		if ($database) $this->database($image);

		return $image;
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS THE IMAGE FILE, GENERATING THUMBS
	////////////////////////////////////////////////////////////////////////////
	public function process(&$image, $name=false, $hash=false) {
		//ONLY SUPPORT JPEG / PNG / GIF FILES FOR NOW
		switch (strtoupper($image->getImageFormat())) {
			case 'JPEG':
			case 'JPG':
			case 'PNG':
			case 'GIF':
				// DO NOTHING!
			break;

			default:
				throw new \af\exception\import('invalid file format');
		}

		if (empty($name)) $name = $image->getFilename();

		//BUILD DATA TO RETURN
		$data = [
			'average'		=> $this->average($image),
			'exif'			=> $this->getExif($image),
			'hash'			=> $this->getHash($image, $hash),
			'size'			=> $image->getImageLength(),
			'mime'			=> $image->getImageMimeType(),
			'type'			=> $image->getImageFormat(),
			'name'			=> basename($name),
		];

		//TODO: ADD MIME TYPE TO THIS CALL SO WE HAVE PROPER EXTENSION!
		$data['url']		= $this->af->url->cdn($data['hash']);
		$data['file_hash']	= hex2bin($data['hash']);


		//SAVE THE FILE TO OUR STORAGE SYSTEM
		$this->write($data['hash'], $image->getImageBlob());


		//RORATE THE IMAGE, IF NEEDED
		switch ($image->getImageOrientation()) {
			case \Imagick::ORIENTATION_UNDEFINED:	break;	//NOT SET
			case \Imagick::ORIENTATION_TOPLEFT:		break;	//ALREADY CORRECT

			case \Imagick::ORIENTATION_TOPRIGHT:
				$image->flopImage();
			break;

			case \Imagick::ORIENTATION_BOTTOMRIGHT:
				$image->rotateImage('#fff', 180);
			break;

			case \Imagick::ORIENTATION_BOTTOMLEFT:
				$image->flipImage();
			break;

			case \Imagick::ORIENTATION_LEFTTOP:
				$image->transposeImage();
			break;

			case \Imagick::ORIENTATION_RIGHTTOP:
				$image->rotateImage('#fff', 90);
			break;

			case \Imagick::ORIENTATION_RIGHTBOTTOM:
				$image->transverseImage();
			break;

			case \Imagick::ORIENTATION_LEFTBOTTOM:
				$image->rotateImage('#fff', -90);
			break;
		}

		//RESET ORIENTATION INFORMATION
		$image->setImageOrientation(\Imagick::ORIENTATION_TOPLEFT);


		//PULL WIDTH AND HEIGHT AFTER ORIENTATION FIX
		$data += [
			'file_width'	=> $image->getImageWidth(),
			'file_height'	=> $image->getImageHeight(),
		];


		//SET'S THE IMAGES COLOR SPACE TO SRGB - ANDROID/IOS COMPATIBLITY
		try {
			$image->profileImage('icc',
				file_get_contents(__DIR__.'/../srgb.icc')
			);
		} catch (\ImagickException $exception) {}

		//GET THE LONG EDGE OF THE IMAGE
		$long = max($data['file_width'], $data['file_height']);

		//PROCESS RESIZED IMAGES (MAINTAIN ASPECT RATIO)
		//NOTE: CREATE SMALLEST SIZE REGARDLESS, IN CASE OF ROTATION/ICC ISSUES
		foreach ($this->imageSize as $size => $quality) {
			if ($size >= $long  &&  $size !== $this->smallest) continue;
			$data[$size] = $this->resize($image, $size, false, $quality);
		}

		//PROCESS THUMBNAILS (SQUARE CROP)
		foreach ($this->thumbSize as $size) {
			$data[$size] = $this->resize($image, $size, true);
		}

		//CLEANUP / FREE RESOURCES
		$image->clear();

		//RETURN THE EVERYTHINGS!
		return $data;
	}




	////////////////////////////////////////////////////////////////////////////
	// FINE THE AVERAGE COLOR OF THE IMAGE
	////////////////////////////////////////////////////////////////////////////
	public function average(&$image) {
		try {
			//RESIZE
			$resized = clone $image;
			$resized->scaleimage(50, 50);
			$resized->posterizeImage(200, false);

			$ar = $ag = $ab = 0;
			$colors = [];

			for ($y=0; $y<50; $y++) {
				for ($x=0; $x<50; $x++) {
					$pixel			= $resized->getImagePixelColor($x, $y);
					$list			= $pixel->getColor();
					list($r,$g,$b)	= array_values($list);
					$pixel->clear();

					$ar += $r;
					$ag += $g;
					$ab += $b;

					if ($r < 0x08  &&  $g < 0x08  &&  $b < 0x08)		continue;
					if ($r > 0xf8  &&  $g > 0xf8  &&  $b > 0xf8)		continue;
					if (abs($r - $g) < 0x08  &&  abs($r - $b) < 0x08)	continue;

					$rgb = (($r&0xFF)<<16) | (($g&0xFF)<<8) | ($b&0xFF);

					if (empty($colors[$rgb])) $colors[$rgb] = 0;
					$colors[$rgb]++;
				}
			}

			//RETURN MOST POPULAR COLOR
			if (!empty($colors)) {
				$color	= array_keys($colors, max($colors));
				$color	= $color[0];

			} else {
				$ar		= (int) $ar / (50*50);
				$ag		= (int) $ag / (50*50);
				$ab		= (int) $ab / (50*50);
				$color	= (($ar&0xFF)<<16) | (($ag&0xFF)<<8) | ($ab&0xFF);
			}

			return str_pad(dechex($color), 6, '0', STR_PAD_LEFT);

		} catch (\ImagickException $e) {}

		return NULL;
	}




	////////////////////////////////////////////////////////////////////////////
	// RESIZE THE IMAGE
	////////////////////////////////////////////////////////////////////////////
	public function resize(&$image, $size, $square=false, $quality=true) {
		$resized = clone $image;

		//RESIZE THE IMAGE
		if ($square) {
			$resized->cropThumbnailImage($size, $size);
		} else {
			$resized->resizeImage($size, $size, \Imagick::FILTER_SINC, 1, true);
		}

		//QUALITY SETTING
		if (is_bool($quality)) {
			$quality = 85;

		} else {
			$resized->stripImage();
			try {
				$resized->profileImage('icc',
					file_get_contents(__DIR__.'/../srgb.icc')
				);
			} catch (\ImagickException $exception) {}
		}


		//SET NEW IMAGE PROPERTIES
		$resized->setImagePage(0, 0, 0, 0);
		$resized->setImageCompressionQuality($quality);

		//GET BINARY DATA
		$blob = $resized->getImageBlob();

		//BUILD OUR DATA ARRAY
		$data['type']		= (string) $size;
		$data['hash']		= md5($blob);
		$data['size']		= strlen($blob);
		$data['url']		= $this->af->url->cdn($data['hash']);
		$data['thumb_hash']	= $this->unhex($data['hash']);

		//COPY BLOB TO IMAGE FILE SERVER
		$data['good']		= $this->write($data['hash'], $blob);

		//CLEANUP / FREE RESOURCES
		$resized->clear();

		//RETURN OUR DATA ARRAY!
		return $data;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE EXIF DATA FROM THE IMAGE
	////////////////////////////////////////////////////////////////////////////
	public function getExif(&$image) {
		$path = $image->getFilename();

		//LOCAL FILE, MUCH FASTER TO USE FILE SYSTEM THAN TO GENERATE A BLOB
		if (!empty($path)) {
			if (substr($path, 0, 1) === '/'  ||  substr($path, 0, 7) === 'file://') {
				return $this->exif_read_data($path);
			}
		}

		return $this->exif_read_data(
			'data://' . $image->getImageMimeType() .
			';base64,' . base64_encode(
				substr($image->getImageBlob(), 0, AF_MEGABYTE)
			)
		);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE FILE HASH
	////////////////////////////////////////////////////////////////////////////
	public function getHash(&$image, $hash=false) {
		if (!empty($hash)) {
			if (is_string($hash)) 					return $this->unhex($hash);
			if (tbx_array($hash)) {
				if (!empty($hash['hash']))			return $this->unhex($hash['hash']);
				if (!empty($hash['file_hash']))		return $this->unhex($hash['file_hash']);
				if (!empty($hash['thumb_hash']))	return $this->unhex($hash['thumb_hash']);
			}
		}

		$path = $image->getFilename();

		//LOCAL FILE, MUCH FASTER TO USE FILE SYSTEM THAN TO GENERATE A BLOB
		if (!empty($path)) {
			if (substr($path, 0, 1) === '/'		||
				substr($path, 0, 3) === '../'	||
				substr($path, 0, 7) === 'file://') {

				//	TODO:	MD5 SUCKS, REPLACE IS.
				//			BUT NEED TO UPDATE ENTIRE DATABASE FIRCE
				if (\af\file::readable($path))		return md5_file($path);
			}
		}

		return md5($image->getImageBlob());
	}




	////////////////////////////////////////////////////////////////////////////
	// CONVERT A HEX STRING TO BINARY
	////////////////////////////////////////////////////////////////////////////
	public function unhex($hash) {
		return (ctype_xdigit($hash)  &&  !(strlen($hash)%2))
			? bin2hex($hash)
			: $hash;
	}




	////////////////////////////////////////////////////////////////////////////
	// READ THE EXIF DATA FROM A FILE
	////////////////////////////////////////////////////////////////////////////
	public function exif_read_data($source) {
		$exif = @exif_read_data($source);
		if (empty($exif)) return false;

		//NOTE: PROPIETARY BINARY DATA, JUST REMOVE IT
		unset($exif['MakerNote']);

		foreach (static::$exif as $key => $item) {
			if (isset($exif['UndefinedTag:0x'.$key])) {
				$exif[$item] = $exif['UndefinedTag:0x'.$key];
				unset($exif['UndefinedTag:0x'.$key]);
			}
		}

		foreach ($exif as $key => &$item) {
			if (substr($key, 0, 15) === 'UndefinedTag:0x') {
				unset($exif[$key]);
			} else {
				$this->exifClean($item);
			}
		} unset($item);

		return $exif;
	}




	////////////////////////////////////////////////////////////////////////////
	// CLEAN A SINGLE EXIF VALUE
	////////////////////////////////////////////////////////////////////////////
	protected function exifClean(&$data) {
		if (is_string($data)) {
			$data = afString::toUtf8($data, 'UTF-8');
			$data = preg_replace('/[\x00-\x1F]/', ' ', $data);
			$data = afString::doubletrim($data);

		} else if (tbx_array($data)) {
			foreach ($data as &$item) {
				$this->exifClean($item);
			} unset($item);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// WRITE FILE TO DISK
	////////////////////////////////////////////////////////////////////////////
	public function write($hash, $blob) {
		$path = $this->af->path() . 'files/' . $this->af->url->cdnPath($hash);

		if (!\af\path::create(substr($path, 0, -32))) {
			throw new \af\exception\import('Unable to create path');
		}

		$return = @file_put_contents($path, $blob, LOCK_EX);

		// ZERO BYTES WRITTEN IS ALSO A FAILURE
		if (empty($return)) {
			throw new \af\exception\import('Unable to save image file');
		}

		return $return;
	}




	////////////////////////////////////////////////////////////////////////////
	// ADD FILE DATA TO DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function database($data, $transaction=false) {
		if (empty($data)) return;

		if (empty($this->pudl)) {
			throw new \af\exception\import('PUDL instance not specified');
		}

		if ($transaction) $this->pudl->begin();

		//GET INTERNAL MIME TYPE DATA
		$mime = $this->pudl->rowId('mimetype', 'mime_type', $data['mime']);

		//INSERT MAIN IMAGE
		$this->pudl->exsert('file', [
			'file_hash'				=> $this->unhex($data['hash']),
			'file_size'				=> $data['size'],
			'file_uploaded'			=> $this->pudl->time(),
			'file_name'				=> $data['name'],
			'file_width'			=> $data['file_width'],
			'file_height'			=> $data['file_height'],
			'file_average'			=> $data['average'],
			'mime_id'				=> !empty($mime) ? $mime['mime_id'] : NULL,
		]);

		//INSERT EXIF DATA
		if (!empty($data['exif'])) {
			$this->pudl->exsert('file_meta', [
				'file_hash'			=> $this->unhex($data['hash']),
				'file_meta_name'	=> 'exif',
				'file_meta_value'	=> $data['exif'],
			]);
		}

		//ASSOCIATE FILE WITH CURRENT USER, IF AVAILABLE
		if (!empty($user['user_id'])) {
			$this->pudl->exsert('file_user', [
				'file_hash'			=> $this->unhex($data['hash']),
				'user_id'			=> $user['user_id'],
				'user_time'			=> $this->pudl->time(),
			]);
		}

		$this->databaseThumbs($data);

		if ($transaction) $this->pudl->commit();
	}




	////////////////////////////////////////////////////////////////////////////
	// INSERT THUMBNAIL IMAGES
	////////////////////////////////////////////////////////////////////////////
	protected function databaseThumbs($data) {
		if (empty($data)) return;

		foreach ($data as $key => $item) {
			if (((int)$key) < 1  ||  !tbx_array($item)) continue;

			$this->pudl->exsert('file_thumb', [
				'thumb_hash'		=> $this->unhex($item['hash']),
				'file_hash'			=> $this->unhex($data['hash']),
				'thumb_size'		=> $item['size'],
				'thumb_type'		=> $item['type'],
			]);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	protected		$altaform	= NULL;
	protected		$pudl		= NULL;
	public			$thumbSize	= [50, 100, 150, 200];
	public			$imageSize	= [500=>50, 800=>true, 1920=>true];
	public			$smallest	= 500;




	////////////////////////////////////////////////////////////////////////////
	// EXIF CODES THAT NEED FIXING
	////////////////////////////////////////////////////////////////////////////
	public static	$exif = [
		'A430' => 'CameraOwnerName',
		'A431' => 'BodySerialNumber',
		'A432' => 'LensSpecification',
		'A433' => 'LensMake',
		'A434' => 'LensModel',
		'A435' => 'LensSerialNumber',
		'8830' => 'SensitivityType',
		'8832' => 'RecommendedExposureIndex',
	];

}
