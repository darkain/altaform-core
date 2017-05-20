<?php

require_once('_altaform/upload.php.inc');


$user->requireLogin();


$data = fileImporter::upload();
assert500(
	is_array($data),
	'Unable to process image file - ' . fileImporter::error()
);


//TODO: STR_REPLACE IS A TEMPORARY FIX.
//THE UPLOADER SHOULD RECOGNIZE OUR CDN LOCATION BY DEFAULT, BUT DOESNT YET
if (!empty($data[500]['url'])) {
	return $af->json([
		'location' => str_replace('cdn/', 'files/', $data[500]['url']),
	]);
}


//TODO: OUTPUT SHOULD OPTIONALLY BE JSON FOR TINYMCE, OR AF STYLE FOR COSPIX
if (!empty($data['url'])) {
	return $af->json([
		'location' => str_replace('cdn/', 'files/', $data['url']),
	]);
}


error500('Unknown error while processing image file');
