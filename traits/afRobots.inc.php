<?php


/*
Reference:
https://developers.google.com/webmasters/control-crawl-index/docs/robots_meta_tag
*/


trait afRobots {

	public function robots($flags) {
		if (is_string($flags)) $flags = explode(',', $flags);

		foreach ($flags as $item) {
			$item = trim((string)$item);
			$this->_robots[$item] = $item;
		}

		return $this;
	}


	public function noindex()		{ return $this->robots('noindex');		}
	public function nofollow()		{ return $this->robots('nofollow');		}
	public function noarchive()		{ return $this->robots('noarchive');	}
	public function nosnippet()		{ return $this->robots('nosnippet');	}
	public function noodp()			{ return $this->robots('noodp');		}
	public function notranslate()	{ return $this->robots('notranslate');	}
	public function noimageindex()	{ return $this->robots('noimageindex');	}


	private $_robots = [];

}
