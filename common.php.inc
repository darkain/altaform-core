<?php

function _pudl_thumb($table, $field, $thumbsize) {
	return array(
		$table,
		array(
			'join' => array('th' => 'pudl_file_thumb'),
			'clause' => array(
				$field.'=th.file_hash',
				"th.thumb_type='$thumbsize'",
			)
		)
	);
}



function _pudl_user($thumbsize) {
	return _pudl_thumb('pudl_user', 'us.user_icon', $thumbsize);
}

function _pudl_event($thumbsize) {
	return _pudl_thumb('pudl_event', 'ev.event_icon', $thumbsize);
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
