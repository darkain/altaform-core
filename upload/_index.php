<?php

require_once('_altaform/upload.php.inc');


$user->requireLogin();


$data = fileImporter::upload();
assert500(
	is_array($data),
	'Unable to process image file - ' . fileImporter::error()
);


$path = false;


if (!empty($data[500]['url'])) {
	$path = $data[500]['url'];
}


if (!empty($data['url'])) {
	$path = $data['url'];
}


assert500($path, 'Unknown error while processing image file');


if (!empty($afconfig->cdn['local'])) {
	$path = str_replace('cdn/', $afconfig->cdn['local'].'/', $path);
}

return $af->json(['location' => $path]);
