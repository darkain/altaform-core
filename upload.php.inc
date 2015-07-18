<?php

require_once('amazon.php.inc');
require_once('_exif/src/PelJpeg.php');


class fileImporter {

	public static function getMimeType($ext) {
		global $db;

		$ext = strtolower($ext);

		if (!empty($db)) {
			$row = $db->rowId('pudl_mimetype', 'mime_ext', $db->safe($ext));
			if (!empty($row)) return $row;
		}

		$mimetype = [
			'mime_id'	=> NULL,
			'mime_ext'	=> $ext,
			'mime_type'	=> 'application/octet-stream',
		];

		switch ($ext) {
			case 'jpe':
			case 'jpg': $ext = 'jpeg';
			case 'jpeg':
			case 'tiff':
			case 'gif':
			case 'png':
				$mimetype['mime_type'] = 'image/'.$ext;
		}

		return $mimetype;
	}



	public static function import($path, $data=false, $database=true) {
		global $af, $db, $site, $user;

		if (empty($data)) $data = [];

		//GET THE FILE HASH
		if (empty($data['hash'])  ||  isset($data['parent'])) {
			$data['hash'] = md5_file($path);
		}

		self::$hash = $data['hash'];
		$base = $af->path();

		//GET THE FILE NAME
		if (empty($data['name'])) {
			$info = pathinfo($path);
			$data['name'] = $info['basename'];
		}

		//GET THE FILE EXTENSION
		if (empty($data['ext'])) {
			$data['ext'] = '';
			$pos = strrpos($data['name'], '.');
			if ($pos !== false) $data['ext'] = substr($data['name'], $pos+1);
			$data['ext'] = strtolower($data['ext']);
		}

		//get the MIMETYPE information for the file
		if (empty($data['mime'])) {
			$data['mime'] = self::getMimeType($data['ext']);
		}

		//IMPORT INTO AMAZON WEB SERVICES (AWS) S3 STORAGE
		s3upload(
			$path,
			$site['aws_bucket'],
			afurl::cdnPath($data['hash']),
			$data['mime']['mime_type'],
			array(
				'CacheControl'	=> 'max-age=315360000, public',
				'Expires'		=> gmdate("D, d M Y H:i:s T", strtotime("+10 years")),
			)
		);

		//setup data to be inserted into the database
		if (empty($data['parent'])) {
			$insert = [
				'file_hash'		=> pudlFunction::unhex($data['hash']),
				'file_size'		=> filesize($path),
				'file_uploaded'	=> $db->time(),
				'file_name'		=> $data['name'],
				'mime_id'		=> $data['mime']['mime_id'],
			];

			//insert file information into database
			if ($database) {
				$db->insert('pudl_file', $insert, true, ['mime_id'=>$insert['mime_id']]);

				//link image to current user account
				if (!empty($user['user_id'])) {
					$db->insert('pudl_file_user', [
						'file_hash'		=> pudlFunction::unhex($data['hash']),
						'user_id'		=> $user['user_id'],
						'user_time'		=> $db->time(),
					], false, 'file_hash=file_hash');
				}
			}

		} else {
			$insert = [
				'thumb_hash'	=> pudlFunction::unhex($data['hash']),
				'file_hash'		=> $data['parent'],
				'thumb_size'	=> filesize($path),
				'thumb_type'	=> (string)$data['thumb_type'],
			];

			//insert file information into database
			if ($database) {
				$db->insert('pudl_file_thumb', $insert, false, 'thumb_hash=thumb_hash');
			}
		}

		if ($data['mime'] !== false) $insert += $data['mime'];
		$insert['file_cdn'] = $path;
		$insert['file_url'] = $af->cdnUrl($data['hash']);
		$insert['hash'] = $data['hash'];

		return $insert;
	}



	public static function importImage($path, $data=false, $database=false) {
		global $db;

		$data = self::import($path, $data, $database);
		if (empty($data)) return false;

		//generate thumbnail if uploaded file is image!
		if (substr($data['mime_type'], 0, 6) !== 'image/') return $data;

		$data['parent'] = $data['file_hash'];

		$temp = tempnam(sys_get_temp_dir(), 'thumb-');
		unlink($temp);
		$temp .= '.' . $data['mime_ext'];


		//RESIZED IMAGES
		$resize = self::resize($data['file_cdn'], $temp);
		if (is_array($resize)) {
			// PUT EXIF DATA INTO DATABASE
			if (!empty($resize['exif'])) {
				$data['exif'] = $resize['exif'];

				$encode = pudlFunction::jsonEncode($resize['exif']);

				//TODO: CONVERT THIS OVER TO NEW DYNAMIC COLUMNS
				if (!empty($encode)) {
					$db->insert('pudl_file_meta', [
						'file_hash'			=> $data['file_hash'],
						'file_meta_name'	=> 'exif',
						'file_meta_value'	=> $encode,
					], true, 'file_hash=file_hash');
				}
			}

			foreach (self::$imageSize as $size) {
				if (is_string($resize[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = self::import($resize[$size], $data);
					@unlink($resize[$size]);
				}
			}
		}

		//CROPPED THUMBNAILS
		$crop = self::thumbnail($data['file_cdn'], $temp);
		if (is_array($crop)) {
			foreach (self::$thumbSize as $size) {
				if (is_string($crop[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = self::import($crop[$size], $data);
					@unlink($crop[$size]);
				}
			}
		}

		return $data;
	}



	public static function importFacebookProfileImage($fbid) {
		$fburl = "http://graph.facebook.com/v2.4/$fbid/picture?width=9999&height=9999";

		$data = @file_get_contents($fburl);
		if ($data === false) return false;

		$temp = tempnam(sys_get_temp_dir(), 'fb-') . '.jpg';

		file_put_contents($temp, $data);

		$data = self::importImage($temp, true);

		@unlink($temp);

		return $data;
	}



	public static function process($formname, $data=false) {
		//verify variables
		if ( empty($_FILES[$formname])) return false;
		if (!empty($_FILES[$formname]['error'])) return false;
		if ( empty($_FILES[$formname]['size' ])) return false;

		//setup path/file name variables
		if (empty($data)) $data = [];
		$data['path'] = $_FILES[$formname]['tmp_name'];
		$data['name'] = $_FILES[$formname]['name'];

		return self::importImage($data['path'], $data, true);
	}



	public static function thumbnail($source, $output, $sizes=false, $quality=90, $squared=true) {
		$image = @getimagesize($source);
		if ($image === false) return '0x0001';

		if ($image[0] <= 0 || $image[1] <= 0) return '0x0002';

		$data = false;
		$rotate = 1;
		switch (strtolower(preg_replace('/^.*?\//', '', $image['mime']))) {
			case 'jpeg':
			case 'jpe':
			case 'jpg':
				$jpeg	= self::loadJpeg($source);
				$data	= $jpeg['image'];
				$image['exif'] = $jpeg['exif'];
				break;

			case 'png': $data = @ImageCreateFromPNG($source); break;

			case 'gif': $data = @ImageCreateFromGIF($source); break;
		}

		if (empty($data)) return '0x0003';

		if ($squared) {
			if ($image[0] > $image[1]) {
				$x = ($image[0] - $image[1]) / 2;
				$y = 0;
				$square = $image[0] - ($x * 2);
			} else {
				$x = 0;
				$y = ($image[1] - $image[0]) / 2;
				$square = $image[1] - ($y * 2);
			}
		} else {
			if ($image[0] < $image[1]) {
				$x = ($image[0] - $image[1]) / 2;
				$y = 0;
				$w = $image[0] / $image[1];
				$h = 1.0;
				$square = $image[0] - ($x * 2);
			} else {
				$x = 0;
				$y = ($image[1] - $image[0]) / 2;
				$w = 1.0;
				$h = $image[1] / $image[0];
				$square = $image[1] - ($y * 2);
			}
		}

		if (empty($sizes)) $sizes = self::$thumbSize;
		if (!is_array($sizes)) $sizes = [$sizes];
		foreach ($sizes as $size) {
			$size = (int) $size;
			if ($size < 1) continue;

			$return = '0x0004';
			$canvas = @ImageCreateTrueColor($size, $size);
			if (empty($canvas)) {
				@ImageDestroy($data);
				return $return;
			}

			//Set the image to WHITE instead of BLACK
			$color = @ImageColorAllocate($canvas, 255, 255, 255);
			@ImageFill($canvas, 0, 0, $color);

			//NEW FILENAME OMGZ!
			$image[$size] = preg_replace('/^.*\./', '$0x'.$size.'.', $output);

			$ok = false;
			if ($squared) {
				$ok = @ImageCopyResampled($canvas, $data, 0, 0, $x, $y, $size, $size, $square, $square);
			} else {
				$ok = @ImageCopyResampled(
					$canvas, $data,
					($size-($size*$w))/2,	($size-($size*$h))/2,	0,			0,
					$size*$w,				$size*$h,				$image[0],	$image[1]
				);
			}

			if ($ok) switch( strtolower(preg_replace('/^.*\./', '', $output)) ) {
				case 'jpeg':
				case 'jpe':
				case 'jpg':
					//USE PEL TO EXTRACT ICC DATA FROM SOURCE, AND EMBED IN DESTINATION
					$canvas = self::rotate($canvas, $jpeg);
					$return = new PelJpeg($canvas);
					if (!empty($jpeg['icc'])) $return->setICC($jpeg['icc']);
					$return->saveFile($image[$size]);
					break;

				case 'png': $return = @ImagePNG($canvas, $image[$size]); break;

				case 'gif': $return = @ImageGIF($canvas, $image[$size]); break;

				default: return '0x0005 - ' . $output;
			}
			@ImageDestroy($canvas);
		}

		@ImageDestroy($data);

		if ($return === false) return '0x0007';

		return $image;
	}



	public static function resize($source, $output, $sizes=false, $quality=90) {

		$image = @getimagesize($source);
		if ($image === false) return '0x0001';

		if ($image[0] <= 0 || $image[1] <= 0) return '0x0002';


		//LOAD UP THE SOURCE IMAGE FILE
		$data = false;
		$rotate = 1;
		switch (strtolower(preg_replace('/^.*?\//', '', $image['mime']))) {
			case 'jpeg':
			case 'jpe':
			case 'jpg':
				$jpeg	= self::loadJpeg($source);
				$data	= $jpeg['image'];
				$image['exif'] = $jpeg['exif'];
				break;

			case 'png': $data = @ImageCreateFromPNG($source); break;

			case 'gif': $data = @ImageCreateFromGIF($source); break;
		}

		if ($data === false) return '0x0003';
		$return = '0x0008';


		//LOOP THRU EACH SIZE, AND CREATE RESIZED IMAGE FOR EACH
		if (empty($sizes)) $sizes = self::$imageSize;
		if (!is_array($sizes)) $sizes = array($sizes);

		foreach ($sizes as $size) {
			$size = (int) $size;
			if ($size < 1) continue;

			if ($image[0] > $image[1]) {
				$scale = $size / $image[0];
			} else {
				$scale = $size / $image[1];
			}

			if ($scale >= 1) {
				$image[$size] = true;
				continue;
			}

			$w = (int)($image[0] * $scale);
			$h = (int)($image[1] * $scale);


			$return = '0x0004';
			$canvas = @ImageCreateTrueColor($w, $h);
			if ($canvas === false) {
				@ImageDestroy($data);
				return $return;
			}

			//NEW FILENAME OMGZ!
			$image[$size] = preg_replace('/^.*\./', '$0x'.$size.'.', $output);

			if (@ImageCopyResampled($canvas, $data, 0, 0, 0, 0, $w, $h, $image[0], $image[1])) {
				switch( strtolower(preg_replace('/^.*\./', '', $image[$size])) ) {
					case 'jpeg':
					case 'jpe':
					case 'jpg':
						//USE PEL TO EXTRACT EXIF DATA FROM SOURCE, AND EMBED IN DESTINATION
						$canvas = self::rotate($canvas, $jpeg);
						$return = new PelJpeg($canvas);
						if (!empty($jpeg['icc'])) $return->setICC($jpeg['icc']);
						$return->saveFile($image[$size]);
						break;

					//OTHER FORMATS, NOTHING SPECIAL
					case 'png': $return = @imagepng($canvas, $image[$size]); break;
					case 'gif': $return = @imagegif($canvas, $image[$size]); break;
					default: return '0x0005 - ' . $image[$size];
				}
			}
			@imagedestroy($canvas);
		}


		@imagedestroy($data);

		if ($return === false) return '0x0007';

		return $image;
	}



	protected static function rotate($canvas, $rotate) {
		if (is_array($rotate)) $rotate = $rotate['rotate'];
		$tmp = $canvas;

		switch ($rotate) {
			//case 1: do nothing, as image orientation is already correct

			case 2: ImageFlip($canvas, IMG_FLIP_HORIZONTAL); break;

			case 4: ImageFlip($canvas, IMG_FLIP_VERTICAL); break;

			case 3: ImageFlip($canvas, IMG_FLIP_BOTH); break;

			case 7:
				ImageFlip($canvas, IMG_FLIP_HORIZONTAL);
				//NO BREAK; FALL INTO ROTATE #6

			case 6:
				$canvas = ImageRotate($tmp, 270, 0);
				ImageDestroy($tmp);
			break;

			case 5:
				ImageFlip($canvas, IMG_FLIP_HORIZONTAL);
				//NO BREAK; FALL INTO ROTATE #8

			case 8:
				$canvas = ImageRotate($tmp, 90, 0);
				ImageDestroy($tmp);
			break;
		}

		return $canvas;
	}



	protected static function loadJpeg($source) {
		$data = [
			'image'		=> false,
			'raw'		=> false,
			'icc'		=> false,
			'rotate'	=> 1,
		];

		try {
			$pel = new PelJpeg($source);

			$data['image']	= @ImageCreateFromString($pel->getBytes());
			$data['raw']	= $pel->getExif();
			$data['icc']	= $pel->getICC();

			if (!empty($data['raw'])) {
				$tiff	= $data['raw']->getTiff();
				$ifd0	= $tiff->getIfd();
				$entry	= $ifd0->getEntry(PelTag::ORIENTATION);

				if (!empty($entry)) {
					$data['rotate'] = $entry->getValue();
					$entry->setValue(1);
					$ifd0->addEntry($entry);
					$data['raw']->setTiff($tiff);
				}
			}

		} catch (Exception $e) {
			_af_email_error('IMAGE ERROR:' . self::$hash);
			$data['image'] = @ImageCreateFromJPEG($source);
		}

		$data['exif'] = self::exif_read_data($source);

		return $data;
	}



	public static function exif_read_data($source) {
		$exif = @exif_read_data($source);
		if (empty($exif)) return false;

		//NOTE: This tag is broken, so just remove it
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

		foreach ($fix as $key => $val) {
			if (isset($exif['UndefinedTag:0x'.$key])) {
				$exif[$val] = $exif['UndefinedTag:0x'.$key];
				unset($exif['UndefinedTag:0x'.$key]);
			}
		}

		return $exif;
	}



	public static $thumbSize = array(50, 100, 150, 200);
	public static $imageSize = array(800, 1280, 1920);
	public static $hash = '';
}
