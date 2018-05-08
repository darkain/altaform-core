<?php

require_once('_altaform/upload.php.inc');


$user->requireLogin();


$data = afUpload::upload();
assert500(
	tbx_array($data),
	'Unable to process image file - ' . afUpload::error()
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
