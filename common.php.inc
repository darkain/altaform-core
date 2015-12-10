<?php

function _pudl_thumb($table, $field, $thumbsize=200) {
	return [
		998 => $table,
		999 => [
			'left' => ['th' => 'pudl_file_thumb'],
			'clause' => [
				'th.file_hash'	=> pudl::column($field),
				'th.thumb_type'	=> (string) $thumbsize,
			]
		]
	];
}



function _pudl_user() {
	return _pudl_thumb('pudl_user', 'us.user_icon');
}

function _pudl_article() {
	return _pudl_thumb('pudl_article', 'ar.article_icon');
}

function _pudl_event() {
	return _pudl_thumb('pudl_event', 'ev.event_icon');
}

function _pudl_gathering() {
	return _pudl_thumb('pudl_gathering', 'gt.gathering_icon');
}

function _pudl_file($thumbsize=200) {
	return _pudl_thumb('pudl_file', 'fl.file_hash', $thumbsize);
}

function _pudl_gallery() {
	return _pudl_thumb('pudl_gallery', 'ga.gallery_thumb');
}

function _pudl_gallery_image($thumbsize=200) {
	return _pudl_thumb('pudl_gallery_image', 'gi.file_hash', $thumbsize);
}

function _pudl_product() {
	return _pudl_thumb('pudl_product', 'pr.product_icon');
}

function _pudl_vendor() {
	return _pudl_thumb('pudl_vendor', 've.vendor_icon');
}

function _pudl_group() {
	return _pudl_thumb('pudl_group', 'gr.group_icon');
}

function _pudl_feature($thumbsize=200) {
	return _pudl_thumb('pudl_feature', 'fe.file_hash', $thumbsize);
}



function _pudl_gallery_costume() {
	$table = _pudl_gallery();

	$table[] = [
		'left' => ['gl1'=>'pudl_group_label'],
		'clause' => 'ga.series_id=gl1.group_label_id'
	];

	$table[] = [
		'left' => ['gl2'=>'pudl_group_label'],
		'clause' => 'ga.character_id=gl2.group_label_id'
	];

	$table[] = [
		'left' => ['gl3'=>'pudl_group_label'],
		'clause' => 'ga.outfit_id=gl3.group_label_id'
	];

	return $table;
}



function af_dump($var, $die=true) {
	echo '<pre>';
	var_export($var);
	echo '</pre>';
	if ($die) exit;
}



//TODO: WARNING: THIS IS DEPRICATED
function url_get_contents($url, $post=[], $followRedirect=true, $failOnError=true, $session=false) {
	return afurl::post($url, $post, [
		CURLOPT_FOLLOWLOCATION	=> $followRedirect,
		CURLOPT_FAILONERROR		=> $failOnError,
	], $session);
}
