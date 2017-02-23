<?php

require_once('_altaform/upload.php.inc');


$user->requireLogin();


assert500(
	$data = fileImporter::upload(),
	'Could not process uploaded image file'
);


//TODO: STR_REPLACE IS A TEMPORARY FIX.
//THE UPLOADER SHOULD RECOGNIZE OUR CDN LOCATION BY DEFAULT, BUT DOESNT YET
if (!empty($data[800]['url'])) {
	return $af->json([
		'location' => str_replace('cdn/', 'files/', $data[800]['url']),
	]);
}


if (!empty($data['url'])) {
	return $af->json([
		'location' => str_replace('cdn/', 'files/', $data['url']),
	]);
}


error500('Unknown error while processing image file');
