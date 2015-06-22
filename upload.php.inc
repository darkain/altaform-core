<?php

require_once('amazon.php.inc');
require_once('_exif/src/PelJpeg.php');


class fileImporter {

	public static function getMimeType($ext) {
		global $db;
		$ext = strtolower($ext);
		$ext = $db->safe($ext);

		$row = false;
		if (!empty($ext)) $row = $db->rowId('pudl_mimetype', 'mime_ext', $ext);

		if (!empty($row)) return $row;

		return [
			'mime_id'	=> NULL,
			'mime_ext'	=> $ext,
			'mime_type'	=> 'application/octet-stream'
		];
	}



	public static function import($path, $data=false) {
		global $af, $db, $site, $user;

		if (empty($data)) $data = [];

		//GET THE FILE HASH
		if (empty($data['md5'])) {
			$data['md5'] = md5_file($path);
			self::$hash = $data['md5'];
		}
		$part1 = substr($data['md5'], 0, 3);
		$part2 = substr($data['md5'], 3, 3);

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
			$data['mime'] = fileImporter::getMimeType($data['ext']);
		}

		//IMPORT INTO AMAZON WEB SERVICES (AWS) S3 STORAGE
		s3upload(
			$path,
			$site['aws_bucket'],
			"$part1/$part2/$data[md5]",
			$data['mime']['mime_type'],
			array(
				'CacheControl'	=> 'max-age=315360000, public',
				'Expires'		=> gmdate("D, d M Y H:i:s T", strtotime("+10 years")),
			)
		);

		//setup data to be inserted into the database
		if (empty($data['parent'])) {
			$insert = array(
				'file_hash'		=> pudlFunction::unhex($data['md5']),
				'file_size'		=> filesize($path),
				'file_uploaded'	=> $db->time(),
				'file_name'		=> $db->safe($data['name']),
				'mime_id'		=> $data['mime']['mime_id'],
			);

			//insert file information into database
			$db->insert('pudl_file', $insert, false, ['mime_id'=>$data['mime']['mime_id']]);

			//link image to current user account
			if (!empty($user['user_id'])) {
				$db->insert('pudl_file_user', array(
					'file_hash'		=> pudlFunction::unhex($data['md5']),
					'user_id'		=> $user['user_id'],
					'user_time'		=> $db->time(),
				), false, 'file_hash=file_hash');
			}

		} else {
			$insert = array(
				'thumb_hash'	=> pudlFunction::unhex($data['md5']),
				'file_hash'		=> $data['parent'],
				'thumb_size'	=> filesize($path),
				'thumb_type'	=> (string)$data['thumb_type'],
			);

			//insert file information into database
			$db->insert('pudl_file_thumb', $insert, false, 'thumb_hash=thumb_hash');
		}

		if ($data['mime'] !== false) $insert += $data['mime'];
		$insert['file_cdn'] = $path;
		$insert['file_url'] = $af->cdnUrl($data['md5']);
		$insert['hash'] = $data['md5'];

		return $insert;
	}



	public static function importImage($path, $data=false) {
		global $db;

		$data = fileImporter::import($path, $data);
		if (empty($data)) return false;

		//generate thumbnail if uploaded file is image!
		if (substr($data['mime_type'], 0, 6) !== 'image/') return $data;

		$data['parent'] = $data['file_hash'];

		$temp = tempnam(sys_get_temp_dir(), 'thumb-');
		unlink($temp);
		$temp .= '.' . $data['mime_ext'];


		//RESIZED IMAGES
		$resize = fileImporter::resize($data['file_cdn'], $temp);
		if (is_array($resize)) {
			// PUT EXIF DATA INTO DATABASE
			if (!empty($resize['exif'])) {
				$encode = @json_encode($resize['exif']);
				if (!empty($encode)) {
					$db->insert('pudl_file_meta', [
						'file_hash'			=> $data['file_hash'],
						'file_meta_name'	=> 'exif',
						'file_meta_value'	=> $db->safe($encode),
					], false, 'file_hash=file_hash');
				}
			}

			foreach (fileImporter::$imageSize as $size) {
				if (is_string($resize[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = fileImporter::import($resize[$size], $data);
					@unlink($resize[$size]);
				}
			}
		}

		//CROPPED THUMBNAILS
		$crop = fileImporter::thumbnail($data['file_cdn'], $temp);
		if (is_array($crop)) {
			foreach (fileImporter::$thumbSize as $size) {
				if (is_string($crop[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = fileImporter::import($crop[$size], $data);
					@unlink($crop[$size]);
				}
			}
		}

		return $data;
	}



	public static function importIcon($path, $data=false) {
		$data = fileImporter::import($path, $data);
		if (empty($data)) return false;

		//generate thumbnail if uploaded file is image!
		if (substr($data['mime_type'], 0, 6) !== 'image/') return $data;

		$data['parent'] = $data['file_hash'];

		$temp = tempnam(sys_get_temp_dir(), 'thumb-') . '.' . $data['mime_ext'];
		$crop = fileImporter::thumbnail($data['file_cdn'], $temp);

		if (is_array($crop)) {
			foreach (fileImporter::$thumbSize as $size) {
				if (is_string($crop[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = fileImporter::import($crop[$size], $data);
					@unlink($crop[$size]);
				}
			}
		}

		return $data;
	}



	//http://stackoverflow.com/questions/2821061/facebook-api-how-do-i-get-a-facebook-users-profile-image-through-the-fb-api
	public static function importFacebookProfileImage($fburl) {
		$fburl = str_replace( 'http://www.facebook.com/', '', $fburl);
		$fburl = str_replace('https://www.facebook.com/', '', $fburl);
		$fburl = "http://graph.facebook.com/v2.2/$fburl/picture?type=large";

		$data = @file_get_contents($fburl);
		if ($data === false) return false;

		$temp = tempnam(sys_get_temp_dir(), 'fb-') . '.jpg';

		file_put_contents($temp, $data);

		$data = fileImporter::importIcon($temp);

		@unlink($temp);

		return $data;
	}



	public static function process($formname, $data=false) {
		//verify variables
		if ( empty($_FILES[$formname])) return false;
		if (!empty($_FILES[$formname]['error'])) return false;
		if ( empty($_FILES[$formname]['size' ])) return false;

		//setup path/file name variables
		if (empty($data)) $data = array();
		$data['path'] = $_FILES[$formname]['tmp_name'];
		$data['name'] = $_FILES[$formname]['name'];

		if (!empty($data['type'])  &&  $data['type'] === 'icon') {
			return fileImporter::importIcon($data['path'], $data);
		}

		return fileImporter::importImage($data['path'], $data);
	}



	public static function thumbnail($src_image, $dest_image, $sizes=false, $quality=90, $squared=true) {
		$image = @getimagesize($src_image);
		if ($image === false) return '0x0001';

		if ($image[0] <= 0 || $image[1] <= 0) return '0x0002';

		$data = false;
		switch (strtolower(preg_replace('/^.*?\//', '', $image['mime']))) {
			case 'jpeg':
			case 'jpg':
				try {
					$exif = new PelJpeg($src_image);
					$data = @ImageCreateFromString($exif->getBytes());
					$iccdata  = $exif->getICC();
				} catch (Exception $e) {
					_af_email_error('IMAGE ERROR:' . self::$hash);
					$data = @imagecreatefromjpeg($src_image);
				}
				break;

			case 'png': $data = @imagecreatefrompng( $src_image); break;
			case 'gif': $data = @imagecreatefromgif( $src_image); break;
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
		if (!is_array($sizes)) $sizes = array($sizes);
		foreach ($sizes as $size) {
			$size = (int) $size;
			if ($size < 1) continue;

			$return = '0x0004';
			$canvas = @imagecreatetruecolor($size, $size);
			if (empty($canvas)) {
				@imagedestroy($data);
				return $return;
			}

			//Set the image to WHITE instead of BLACK
			$color = @imagecolorallocate($canvas, 255, 255, 255);
			@imagefill($canvas, 0, 0, $color);

			//NEW FILENAME OMGZ!
			$image[$size] = preg_replace('/^.*\./', '$0x'.$size.'.', $dest_image);

			$ok = false;
			if ($squared) {
				$ok = @imagecopyresampled($canvas, $data, 0, 0, $x, $y, $size, $size, $square, $square);
			} else {
				$ok = @imagecopyresampled(
					$canvas, $data,
					($size-($size*$w))/2,	($size-($size*$h))/2,	0,			0,
					$size*$w,				$size*$h,				$image[0],	$image[1]
				);
			}

			if ($ok) switch( strtolower(preg_replace('/^.*\./', '', $dest_image)) ) {
				case 'jpeg':
				case 'jpg':
					//USE PEL TO EXTRACT ICC DATA FROM SOURCE, AND EMBED IN DESTINATION
					$return = new PelJpeg($canvas);
					if (!empty($iccdata)) $return->setICC($iccdata);
					$return->saveFile($image[$size]);
					break;

				case 'png': $return = @imagepng($canvas, $image[$size]); break;
				case 'gif': $return = @imagegif($canvas, $image[$size]); break;
				default: return '0x0005 - ' . $dest_image;
			}
			@imagedestroy($canvas);
		}

		@imagedestroy($data);

		if ($return === false) return '0x0007';

		return $image;
	}



	public static function resize($src_image, $dest_image, $sizes=false, $quality=90) {
		global $af;

		$image = @getimagesize($src_image);
		if ($image === false) return '0x0001';

		if ($image[0] <= 0 || $image[1] <= 0) return '0x0002';


		//LOAD UP THE SOURCE IMAGE FILE
		$data = false;
		switch (strtolower(preg_replace('/^.*?\//', '', $image['mime']))) {
			case 'jpeg':
			case 'jpg':
				try {
					$exif = new PelJpeg($src_image);
					$data = @ImageCreateFromString($exif->getBytes());
					$exifdata = $exif->getExif();
					$iccdata  = $exif->getICC();
					if (!empty($exifdata)) $image['exif'] = $af->exif_read_data($src_image);
				} catch (Exception $e) {
					_af_email_error('IMAGE ERROR:' . self::$hash);
					$data = @imagecreatefromjpeg($src_image);
				}
				break;

			case 'png': $data = @ImageCreateFromPNG($src_image); break;
			case 'gif': $data = @ImageCreateFromGIF($src_image); break;
		}

		if ($data === false) return '0x0003';
		$return = '0x0008';


		//LOOP THRU EACH SIZE, AND CREATE RESIZED IMAGE FOR EACH
		if (empty($sizes)) $sizes = fileImporter::$imageSize;
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
			$image[$size] = preg_replace('/^.*\./', '$0x'.$size.'.', $dest_image);

			if (@ImageCopyResampled($canvas, $data, 0, 0, 0, 0, $w, $h, $image[0], $image[1])) {
				switch( strtolower(preg_replace('/^.*\./', '', $image[$size])) ) {
					case 'jpeg':
					case 'jpg':
						//USE PEL TO EXTRACT EXIF DATA FROM SOURCE, AND EMBED IN DESTINATION
						$return = new PelJpeg($canvas);
						if (!empty($exifdata)) $return->setExif($exifdata);
						if (!empty($iccdata )) $return->setICC( $iccdata );
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



	public static $thumbSize = array(50, 100, 150, 200);
	public static $imageSize = array(800, 1280, 1920);
	public static $hash = '';
}
