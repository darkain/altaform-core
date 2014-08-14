<?php

require_once('amazon.php.inc');
require_once('_exif/src/PelJpeg.php');


class cdnImporter {

	public static function getMimeType($ext) {
		global $db;

		$ext = strtolower($ext);

		if (!empty($db)) {
			$row = $db->rowId('pudl_mimetype', 'mime_ext', $db->safe($ext));
			if (!empty($row)) return $row;
		}

		$return = [
			'mime_id'	=> NULL,
			'mime_ext'	=> $ext,
			'mime_type'	=> 'application/octet-stream'];
		];

		switch ($ext) {
			case 'jpe':
			case 'jpg': $ext = 'jpeg';
			case 'jpeg':
			case 'tiff':
			case 'gif':
			case 'png':
				$return['mime_type'] = "image/$ext";
		}

		return $return;
	}




	public static function import($path, $data=false) {
		global $af, $site;

		if (empty($data)) $data = array();

		//GET THE FILE HASH
		if (empty($data['hash'])  ||  isset($data['parent'])) {
			$data['hash'] = md5_file($path);
		}
		$part1 = substr($data['hash'], 0, 3);
		$part2 = substr($data['hash'], 3, 3);

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
			$data['mime'] = cdnImporter::getMimeType($data['ext']);
		}

		//IMPORT INTO AMAZON WEB SERVICES (AWS) S3 STORAGE
		s3upload(
			$path,
			$site['aws_bucket'],
			"$part1/$part2/$data[hash]",
			$data['mime']['mime_type'],
			array(
				'CacheControl'	=> 'max-age=315360000, public',
				'Expires'		=> gmdate("D, d M Y H:i:s T", strtotime("+10 years")),
			)
		);

		//setup data to be inserted into the database
		if (empty($data['parent'])) {
			$insert = array(
				'file_hash'		=> array('UNHEX'=>"'$data[hash]'"),
				'file_size'		=> filesize($path),
				'file_name'		=> $data['name'],
				'mime_id'		=> $data['mime']['mime_id'],
			);
			
		} else {
			$insert = array(
				'thumb_hash'	=> array('UNHEX'=>"'$data[hash]'"),
				'file_hash'		=> $data['parent'],
				'thumb_size'	=> filesize($path),
				'thumb_type'	=> $data['thumb_type'],
			);

		}

		if ($data['mime'] !== false) $insert += $data['mime'];
		$insert['file_cdn'] = $path;
		$insert['file_url'] = $af->cdnUrl($data['hash']);
		$insert['hash'] = $data['hash'];

		return $insert;
	}



	public static function importImage($path, $data=false) {
		$data = cdnImporter::import($path, $data);
		if (empty($data)) return false;

		//generate thumbnail if uploaded file is image!
		if (substr($data['mime_type'], 0, 6) !== 'image/') return $data;

		$data['parent'] = $data['file_hash'];

		$temp = tempnam(sys_get_temp_dir(), 'thumb-');
		unlink($temp);
		$temp .= '.' . $data['mime_ext'];
		
		
		//RESIZED IMAGES
		$resize = cdnImporter::resize($data['file_cdn'], $temp);
		if (is_array($resize)) {
			if (!empty($resize['exif'])) $data['exif'] = $resize['exif'];

			foreach (cdnImporter::$imageSize as $size) {
				if (is_string($resize[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = cdnImporter::import($resize[$size], $data);
					@unlink($resize[$size]);
				}
			}
		}

		//CROPPED THUMBNAILS
		$crop = cdnImporter::thumbnail($data['file_cdn'], $temp);
		if (is_array($crop)) {
			foreach (cdnImporter::$thumbSize as $size) {
				if (is_string($crop[$size])) {
					$data['thumb_type'] = $size;
					$data[$size] = cdnImporter::import($crop[$size], $data);
					@unlink($crop[$size]);
				}
			}
		}

		return $data;
	}





	public static function thumbnail($src_image, $dest_image, $sizes=false, $quality=90) {
		$image = @getimagesize($src_image);
		if ($image === false) return '0x0001';

		if ($image[0] <= 0 || $image[1] <= 0) return '0x0002';

		$data = false;
		$rotate = 1;
		switch (strtolower(preg_replace('/^.*?\//', '', $image['mime']))) {
			case 'jpeg':
			case 'jpg':
				$exif = new PelJpeg($src_image);
				$data = @ImageCreateFromString($exif->getBytes());
				$exifdata = $exif->getExif();
				$iccdata  = $exif->getICC();
				if (!empty($exifdata)) {
					$tiff = $exifdata->getTiff();
					$ifd0 = $tiff->getIfd();
					$entry = $ifd0->getEntry(PelTag::ORIENTATION);
					if (!empty($entry)) $rotate = $entry->getValue();
				}
				break;

			case 'jpg': $data = @imagecreatefromjpeg($src_image); break;
			case 'png': $data = @imagecreatefrompng( $src_image); break;
			case 'gif': $data = @imagecreatefromgif( $src_image); break;
		}

		if (empty($data)) return '0x0003';

		if ($image[0] > $image[1]) {
			$x = ($image[0] - $image[1]) / 2;
			$y = 0;
			$square = $image[0] - ($x * 2);
		} else {
			$x = 0;
			$y = ($image[1] - $image[0]) / 2;
			$square = $image[1] - ($y * 2);
		}

		if (empty($sizes)) $sizes = cdnImporter::$thumbSize;
		if (!is_array($sizes)) $sizes = array($sizes);
		foreach ($sizes as $size) {
			$size = (int) $size;
			if ($size < 1) continue;

			$return = '0x0004';
			$canvas = @imagecreatetruecolor($size, $size);
			if ($canvas === false) {
				@imagedestroy($data);
				return $return;
			}

			//NEW FILENAME OMGZ!
			$image[$size] = preg_replace('/^.*\./', '$0x'.$size.'.', $dest_image);		    

			if (@imagecopyresampled($canvas, $data, 0, 0, $x, $y, $size, $size, $square, $square)) {
				switch( strtolower(preg_replace('/^.*\./', '', $dest_image)) ) {
					case 'jpeg':
					case 'jpg':
						switch ($rotate) { //no case1: as it is not rotated
							case 2: imageflip($canvas, IMG_FLIP_HORIZONTAL); break;
							case 4: imageflip($canvas, IMG_FLIP_VERTICAL); break;

							case 3:
								$tmp = $canvas;
								$canvas = imagerotate($tmp, 180, 0);
								imagedestroy($tmp);
							break;

							case 7:
								imageflip($canvas, IMG_FLIP_HORIZONTAL);  //NO BREAK; FALL INTO ROTATE
							case 6:
								$tmp = $canvas;
								$canvas = imagerotate($tmp, 270, 0);
								imagedestroy($tmp);
							break;

							case 5:
								imageflip($canvas, IMG_FLIP_HORIZONTAL);  //NO BREAK; FALL INTO ROTATE
							case 8:
								$tmp = $canvas;
								$canvas = imagerotate($tmp, 90, 0);
								imagedestroy($tmp);
							break;
						}

						//USE PEL TO EXTRACT ICC DATA FROM SOURCE, AND EMBED IN DESTINATION
						$return = new PelJpeg($canvas);
						if (!empty($iccdata)) $return->setICC($iccdata);
						$return->saveFile($image[$size]);
						break;

					case 'png': $return = @imagepng($canvas, $image[$size]); break;
					case 'gif': $return = @imagegif($canvas, $image[$size]); break;
					default: return '0x0005 - ' . $dest_image;
				}
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
		$rotate = 1;
		switch (strtolower(preg_replace('/^.*?\//', '', $image['mime']))) {
			case 'jpeg':
			case 'jpg':
				$exif = new PelJpeg($src_image);
				$data = @ImageCreateFromString($exif->getBytes());
				$exifdata = $exif->getExif();
				$iccdata  = $exif->getICC();
				if (!empty($exifdata)) {
					$tiff = $exifdata->getTiff();
					$ifd0 = $tiff->getIfd();
					$entry = $ifd0->getEntry(PelTag::ORIENTATION);
					if (!empty($entry)) {
						$rotate = $entry->getValue();
						$entry->setValue(1);
						$ifd0->addEntry($entry);
						$exifdata->setTiff($tiff);
					}

					$image['exif'] = $af->exif_read_data($src_image);
				}
				break;

			case 'png': $data = @ImageCreateFromPNG($src_image); break;
			case 'gif': $data = @ImageCreateFromGIF($src_image); break;
		}

		if ($data === false) return '0x0003';
		$return = '0x0008';


		//LOOP THRU EACH SIZE, AND CREATE RESIZED IMAGE FOR EACH
		if (empty($sizes)) $sizes = cdnImporter::$imageSize;
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
						switch ($rotate) { //no case1: as it is not rotated
							case 2: imageflip($canvas, IMG_FLIP_HORIZONTAL); break;
							case 4: imageflip($canvas, IMG_FLIP_VERTICAL); break;

							case 3:
								$tmp = $canvas;
								$canvas = imagerotate($tmp, 180, 0);
								imagedestroy($tmp);
							break;

							case 7:
								imageflip($canvas, IMG_FLIP_HORIZONTAL);  //NO BREAK; FALL INTO ROTATE
							case 6:
								$tmp = $canvas;
								$canvas = imagerotate($tmp, 270, 0);
								imagedestroy($tmp);
							break;

							case 5:
								imageflip($canvas, IMG_FLIP_HORIZONTAL);  //NO BREAK; FALL INTO ROTATE
							case 8:
								$tmp = $canvas;
								$canvas = imagerotate($tmp, 90, 0);
								imagedestroy($tmp);
							break;
						}

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
}
