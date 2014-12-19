<?php

require_once('_products/lib/AmazonECS.class.php');


class afproduct {
	static function search($search, $key, $category='All', $group='Small,Images') {
		global $site; 

		if (empty(self::$redis)) {
			self::$redis = new Redis();
			if (!self::$redis->connect($site['pudl_redis'], -1, 1)) return false;
			self::$redis->setOption(Redis::OPT_SERIALIZER, Redis::SERIALIZER_PHP);
		}

		$data = self::$redis->get($key);
		if (!empty($data)) return $data;

		if (empty(self::$amazon)) {
			self::$amazon = new AmazonECS($site['aws_key'], $site['aws_secret'], 'com', $site['aws_tag']);
			self::$amazon->returnType(AmazonECS::RETURN_TYPE_ARRAY);
		}

		$data = self::$amazon->category($category)->responseGroup($group)->search($search);

		self::$redis->set($key, $data, (60*60));

		return $data;
	}

	static $redis	= false;
	static $amazon	= false;
}
