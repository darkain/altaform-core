<?php

function _pudl_thumb($table, $field, $thumbsize) {
	return [
		998 => $table,
		999 => [
			'left' => ['th' => 'pudl_file_thumb'],
			'clause' => [
				$field.'=th.file_hash',
				"th.thumb_type='$thumbsize'",
			]
		]
	];
}



function _pudl_user($thumbsize) {
	return _pudl_thumb('pudl_user', 'us.user_icon', $thumbsize);
}

function _pudl_article($thumbsize) {
	return _pudl_thumb('pudl_article', 'ar.article_icon', $thumbsize);
}

function _pudl_event($thumbsize) {
	return _pudl_thumb('pudl_event', 'ev.event_icon', $thumbsize);
}

function _pudl_gathering($thumbsize) {
	return _pudl_thumb('pudl_gathering', 'gt.gathering_icon', $thumbsize);
}

function _pudl_file($thumbsize) {
	return _pudl_thumb('pudl_file', 'fl.file_hash', $thumbsize);
}

function _pudl_gallery($thumbsize) {
	return _pudl_thumb('pudl_gallery', 'ga.gallery_thumb', $thumbsize);
}

function _pudl_gallery_image($thumbsize) {
	return _pudl_thumb('pudl_gallery_image', 'gi.file_hash', $thumbsize);
}

function _pudl_product($thumbsize) {
	return _pudl_thumb('pudl_product', 'pr.product_icon', $thumbsize);
}

function _pudl_vendor($thumbsize) {
	return _pudl_thumb('pudl_vendor', 've.vendor_icon', $thumbsize);
}

function _pudl_group($thumbsize) {
	return _pudl_thumb('pudl_group', 'gr.group_icon', $thumbsize);
}



function _pudl_gallery_costume($thumbsize) {
	$table = _pudl_gallery($thumbsize);

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



function url_get_contents($url, $post=array(), $followRedirect=true, $failOnError=true) {
	$ch = curl_init();
	curl_setopt($ch, CURLOPT_URL,				$url);
	curl_setopt($ch, CURLOPT_AUTOREFERER,		true);
	curl_setopt($ch, CURLOPT_BINARYTRANSFER,	true);
	curl_setopt($ch, CURLOPT_FAILONERROR,		$failOnError);
	curl_setopt($ch, CURLOPT_FOLLOWLOCATION,	$followRedirect);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER,	true);
	curl_setopt($ch, CURLOPT_CONNECTTIMEOUT,	20);
	curl_setopt($ch, CURLOPT_TIMEOUT,			20);
	curl_setopt($ch, CURLOPT_POST,				true);
	curl_setopt($ch, CURLOPT_POSTFIELDS,		$post);
	curl_setopt($ch, CURLOPT_HTTPHEADER,		array('Expect:'));

	$agent = ini_get('user_agent');
	if (is_string($agent)) {
		curl_setopt($ch, CURLOPT_USERAGENT,		$agent);
	}

	$contents			= curl_exec($ch);
	$data				= curl_getinfo($ch);
	$data['error']		= curl_error($ch);
	$data['errno']		= curl_errno($ch);
	$data['content']	= $contents;

	curl_close($ch);

	return $data;
}
