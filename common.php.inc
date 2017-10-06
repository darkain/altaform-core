<?php

function _pudl_thumb($table, $field, $thumbsize=200) {
	return [
		997 => $table,
		998 => [
			'left'	=> ['th' => 'pudl_file_thumb'],
			'on'	=> [
				'th.file_hash'	=> pudl::column($field),
				'th.thumb_type'	=> (string) $thumbsize,
			]
		],
		999 => [
			'left'	=> ['tx' => 'pudl_file'],
			'on'	=> ['tx.file_hash' => pudl::column($field)]
		],
	];
}



function _pudl_user($thumbsize=200) {
	return _pudl_thumb('pudl_user', 'us.user_icon', $thumbsize);
}

function _pudl_article($thumbsize=200) {
	return _pudl_thumb('pudl_article', 'ar.article_icon', $thumbsize);
}

function _pudl_event($thumbsize=200) {
	return _pudl_thumb('pudl_event', 'ev.event_icon', $thumbsize);
}

function _pudl_gathering($thumbsize=200) {
	return _pudl_thumb('pudl_gathering', 'gt.gathering_icon', $thumbsize);
}

function _pudl_file($thumbsize=200) {
	return _pudl_thumb('pudl_file', 'fl.file_hash', $thumbsize);
}

function _pudl_gallery($thumbsize=200) {
	return _pudl_thumb('pudl_gallery', 'ga.gallery_thumb', $thumbsize);
}

function _pudl_gallery_image($thumbsize=200) {
	return _pudl_thumb('pudl_gallery_image', 'gi.file_hash', $thumbsize);
}

function _pudl_product($thumbsize=200) {
	return _pudl_thumb('pudl_product', 'pr.product_icon', $thumbsize);
}

function _pudl_vendor($thumbsize=200) {
	return _pudl_thumb('pudl_vendor', 've.vendor_icon', $thumbsize);
}

function _pudl_group($thumbsize=200) {
	return _pudl_thumb('pudl_group', 'gr.group_icon', $thumbsize);
}

function _pudl_feature($thumbsize=200) {
	return _pudl_thumb('pudl_feature', 'fe.file_hash', $thumbsize);
}



function af_dump($var, $die=true) {
	global $af;

	if ($var instanceof pudlObject) $var = $var->raw();

	if ($af instanceof altaform) {
		$af->contentType('txt');
		var_export($var);
		echo "\n";

	} else {
		echo '<pre>';
		var_export($var);
		echo '</pre>';
	}

	if ($die) exit;
}



function get_ancestors($class) {
	if (empty($class)
		||  !is_object($class)
		||  !is_string($class))
		return [];

	$list = [$class];
	while($class = get_parent_class($class)) $list[] = $class;
	return $list;
}
