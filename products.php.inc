<?php

require_once('_products/lib/AmazonECS.class.php');


class afproduct {
	static function search($search, $key=false, $category='All', $group='Small,Images') {
		global $site, $db;

		if (!$db->redis()) return false;

		if (empty($key)) $key = $search;

		$data = self::cached($key);
		if (!empty($data)) return $data;

		try {
			if (empty(self::$amazon)) {
				self::$amazon = new AmazonECS($site['aws_key'], $site['aws_secret'], 'com', $site['aws_tag']);
				self::$amazon->returnType(AmazonECS::RETURN_TYPE_ARRAY);
			}

			$data = self::$amazon->category($category)->responseGroup($group)->search($search);
		} catch(Exception $e) {}

		$db->redis()->set("aws-product-$key", $data, (60*60*24));

		return $data;
	}


	static function cached($key) {
		global $db;
		if (!$db->redis()) return false;
		$data = $db->redis()->get("aws-product-$key");
		return !empty($data) ? $data : false;
	}


	static function image($product) {
		if (!empty($product['MediumImage']['URL'])) {
			return $product['MediumImage']['URL'];
		} else if (!empty($product['TinyImage']['URL'])) {
			return $product['TinyImage']['URL'];
		} else if (!empty($product['ThumbnailImage']['URL'])) {
			return $product['ThumbnailImage']['URL'];
		} else if (!empty($product['SmallImage']['URL'])) {
			return $product['SmallImage']['URL'];
		} else if (!empty($product['ImageSets']['ImageSet'][0]['MediumImage']['URL'])) {
			return $product['ImageSets']['ImageSet'][0]['MediumImage']['URL'];
		} else if (!empty($product['ImageSets']['ImageSet'][0]['TinyImage']['URL'])) {
			return $product['ImageSets']['ImageSet'][0]['TinyImage']['URL'];
		} else if (!empty($product['ImageSets']['ImageSet'][0]['ThumbnailImage']['URL'])) {
			return $product['ImageSets']['ImageSet'][0]['ThumbnailImage']['URL'];
		} else if (!empty($product['ImageSets']['ImageSet'][0]['SmallImage']['URL'])) {
			return $product['ImageSets']['ImageSet'][0]['SmallImage']['URL'];
		}
		return false;
	}

	static $amazon	= false;
}
