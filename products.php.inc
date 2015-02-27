<?php

require_once('_products/lib/AmazonECS.class.php');


class afproduct {
	static function search($search, $key, $category='All', $group='Small,Images') {
		global $site, $db;

		$data = self::cached($key);
		if (!empty($data)) return $data;

		if (empty(self::$amazon)) {
			self::$amazon = new AmazonECS($site['aws_key'], $site['aws_secret'], 'com', $site['aws_tag']);
			self::$amazon->returnType(AmazonECS::RETURN_TYPE_ARRAY);
		}

		$data = self::$amazon->category($category)->responseGroup($group)->search($search);

		$db->redis()->set("aws-product-$key", $data, (60*60*24));

		return $data;
	}


	static function cached($key) {
		global $db;
		$data = $db->redis()->get("aws-product-$key");
		return !empty($data) ? $data : false;
	}

	static $amazon	= false;
}
