<?php

require_once('_products/lib/AmazonECS.class.php');


class afproduct {

	static function search($search, $key=false, $category='All', $group='Small,Images') {
		global $site, $db;

		if (!$db->redis()) return false;

		if (empty($key)) $key = $search;

		$data = self::cached($key);
		if (!empty($data)) return self::https($data);

		try {
			if (empty(self::$amazon)) {
				self::$amazon = new AmazonECS($site['aws_key'], $site['aws_secret'], 'com', $site['aws_tag']);
				self::$amazon->returnType(AmazonECS::RETURN_TYPE_ARRAY);
			}

			$data = self::$amazon->category($category)->responseGroup($group)->search($search);
		} catch(Exception $e) {}

		try {
			$db->redis()->set("aws-product-$key", $data, (AF_DAY));
		} catch (RedisException $e) {}

		return self::https($data);
	}




	static function cached($key) {
		global $db;
		if (!$db->redis()) return false;
		try {
			$data = $db->redis()->get("aws-product-$key");
		} catch (RedisException $e) {
			return false;
		}
		return !empty($data) ? self::https($data) : false;
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




	//CONVERTS IMAGES URLS OVER TO HTTPS
	static function https($data) {
		global $afurl;

		if (!is_array($data)) return $data;

		if ($afurl->https  &&  !empty($data['Items']['Item'])) {
			foreach ($data['Items']['Item'] as &$item) {
				foreach (self::$sizes as $size) {
					if (!empty($item[$size]['URL'])) {
						$item[$size]['URL'] = str_replace(
							'http://ecx.images-amazon.com/',
							'https://images-na.ssl-images-amazon.com/',
							$item[$size]['URL']
						);
					}

					if (!empty($item['ImageSets']['ImageSet'][$size]['URL'])) {
						$item['ImageSets']['ImageSet'][$size]['URL'] = str_replace(
							'http://ecx.images-amazon.com/',
							'https://images-na.ssl-images-amazon.com/',
							$item['ImageSets']['ImageSet'][$size]['URL']
						);
					}
				}
			} unset($item);
		}

		return $data;
	}




	static $amazon	= false;

	public static $sizes	= [
		'TinyImage',
		'SmallImage',
		'MediumImage',
		'LargeImage',
		'SwatchImage',
		'ThumbnailImage',
	];
}
