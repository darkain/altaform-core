<?php


$user->requireLogin();


$import	= new \af\import($af, $db);
$data	= $importer->upload();

$path = false;


if (!empty($data[500]['url'])) {
	$path = $data[500]['url'];
}


if (!empty($data['url'])) {
	$path = $data['url'];
}


\af\affirm(500,
	$path,
	'Unknown error while processing image file'
);


if (!empty($afconfig->cdn['local'])) {
	$path = str_replace('cdn/', $afconfig->cdn['local'].'/', (string)$path);
}

$af->json(['location' => $path]);
