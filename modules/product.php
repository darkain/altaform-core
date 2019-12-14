<?php

namespace af;


require_once(is_owner('_products/lib/AmazonECS.class.php'));




////////////////////////////////////////////////////////////////////////////////
// AMAZON PRODUCT SEARCH
////////////////////////////////////////////////////////////////////////////////
class product {




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\altaform $af, \pudl $pudl) {
		$this->af	= $af;
		$this->pudl	= $pudl;
	}




	////////////////////////////////////////////////////////////////////////////
	// SEARCH FOR A RELEVANT PRODUCT BASED ON KEY PHRASE
	////////////////////////////////////////////////////////////////////////////
	function search($search, $key=false, $category='All', $group='Small,Images') {
		if (!$this->pudl->redis()) return false;

		if (empty($key)) $key = $search;

		$data = $this->cached($key);
		if (!empty($data)) return $this->https($data);

		// DISABLE ERROR REPORTING IN CASE OF CONNECTION TIMEOUT
		$level = error_reporting(0);

		try {
			if (empty(self::$amazon)) {
				self::$amazon = new \AmazonECS(
					$this->af->config->amazon['id'],
					$this->af->config->amazon['secret'],
					'com',
					$this->af->config->amazon['tag']
				);

				self::$amazon->returnType(
					\AmazonECS::RETURN_TYPE_ARRAY
				);
			}

			$data = self::$amazon
				->category($category)
				->responseGroup($group)
				->search($search);
		} catch(Exception $e) {}

		try {
			$this->pudl->redis()->set("aws-product-$key", $data, (AF_DAY));
		} catch (\RedisException $e) {}

		//TURN ERROR REPORTING BACK ON BECAUSE WE STILL NEED IT OTHERWISE
		error_reporting($level);

		return $this->https($data);
	}




	////////////////////////////////////////////////////////////////////////////
	// GET CACHED CONTENT
	////////////////////////////////////////////////////////////////////////////
	function cached($key) {
		if (!$this->pudl->redis()) return false;
		try {
			$data = $this->pudl->redis()->get("aws-product-$key");
		} catch (\RedisException $e) {
			return false;
		}
		return !empty($data) ? $this->https($data) : false;
	}




	////////////////////////////////////////////////////////////////////////////
	// GET THE IMAGE URL, WITH TONS OF FALLBACKS
	////////////////////////////////////////////////////////////////////////////
	function image($product) {
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




	////////////////////////////////////////////////////////////////////////////
	// CONVERTS IMAGES URLS OVER TO HTTPS
	////////////////////////////////////////////////////////////////////////////
	function https($data) {
		if (!tbx_array($data)) return $data;

		if ($this->af->url->https  &&  !empty($data['Items']['Item'])) {
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




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	protected $af	= NULL;
	protected $pudl	= NULL;




	////////////////////////////////////////////////////////////////////////////
	// STATIC VARIABLES
	////////////////////////////////////////////////////////////////////////////
	static			$amazon		= NULL;

	public static	$sizes		= [
		'TinyImage',
		'SmallImage',
		'MediumImage',
		'LargeImage',
		'SwatchImage',
		'ThumbnailImage',
	];

}
