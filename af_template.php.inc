<?php


trait af_template {


	////////////////////////////////////////////////////////////////////////////
	//LINK A JAVASCRIPT FILE IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function script($path) {
		$this->_script[] = ['path' => $path];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//LINK A CASCADING STYLE SHEET FILE IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function style($path) {
		$this->_style[] = ['path' => $path];
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//EMBED INLINE CASCADING STYLE SHEET IN HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function sheet($styles) {
		$this->_sheet[] = $styles;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//EMBED INLINE CASCADING STYLE SHEET IN HTML HEADER FROM FILE
	////////////////////////////////////////////////////////////////////////////
	public function css($file) {
		$data = @file_get_contents($file, true);
		if ($data === false) {
			trigger_error('Unable to load CSS file: '.$file, E_USER_ERROR);
		}
		$this->_sheet[] = $data;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//ADD EXTRA CUSTOM META TAGS TO A PAGE (SUCH AS TWITTER CARDS)
	////////////////////////////////////////////////////////////////////////////
	public function meta($meta) {
		$this->_meta[] = $meta;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//ADD EXTRA CUSTOM META TAGS TO A PAGE (SUCH AS TWITTER CARDS)
	////////////////////////////////////////////////////////////////////////////
	public function metas($meta) {
		$this->_meta = array_merge($this->_meta, $meta);
		return $this;
	}



	////////////////////////////////////////////////////////////////////////////
	//LOAD A TEMPLATE FILE - OVERRIDE DEFAULT TBX LOADER
	////////////////////////////////////////////////////////////////////////////
	public function load($file) {
		$device	= $file . '.' . afdevice::device();
		if (file_exists($device)) return parent::load($device);

		$pathed	= $this->path() . $device;
		if (file_exists($pathed)) return parent::load($pathed);

		return parent::load($file);
	}




	////////////////////////////////////////////////////////////////////////////
	//NO PARAMS - RENDER HTML HEADER TEMPLATES
	//PARAMS - ADD OPTIONS TO HTML HEADER TEMPLATES (deprecated)
	////////////////////////////////////////////////////////////////////////////
	public function header($key=false, $data=false, $replace=false) {
		if ($key === false) {
			if ($this->jq()) return $this;
			return $this->headerHTML()->headerPage();
		}

		if ($replace  ||  empty($this->_headers[$key])) {
			$this->_headers[$key]	= [$data];
		} else {
			$this->_headers[$key][]	= $data;
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//SET A SPECIFIC HTML HEADER TEMPLATE OPTION
	////////////////////////////////////////////////////////////////////////////
	public function headers($key, $data) {
		$this->_headers[$key] = $data;
		return $this;
	}





	////////////////////////////////////////////////////////////////////////////
	//NO PARAMS - RENDER HTML FOOTER TEMPLATES
	//PARAMS - ADD OPTIONS TO HTML FOOTER TEMPLATES (deprecated)
	////////////////////////////////////////////////////////////////////////////
	public function footer($key=false, $data=false, $replace=false) {
		if ($key === false) {
			if ($this->jq()) return $this;
			return $this->footerPage()->footerHTML();
		}

		if ($replace  ||  empty($this->_footers[$key])) {
			$this->_footers[$key]	= [$data];
		} else {
			$this->_footers[$key][]	= $data;
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//SET A SPECIFIC HTML FOOTER TEMPLATE OPTION
	////////////////////////////////////////////////////////////////////////////
	public function footers($key, $data) {
		$this->_footers[$key] = $data;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER HTML HEADER
	////////////////////////////////////////////////////////////////////////////
	public function headerHTML() {
		global $site;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()  &&  file_exists($root.'/header_html_debug.tpl.'.$device)) {
			$this->load($root.'/header_html_debug.tpl.'.$device);
		} else if ($this->debug()  &&  file_exists($root.'/header_html_debug.tpl')) {
			$this->load($root.'/header_html_debug.tpl');
		} else {
			$this->load($root.'/header_html.tpl');
		}

		$this->field('sheet',	implode(' ',	$this->_sheet));
		$this->field('robots',	implode(', ',	$this->_robots));
		$this->block('script',	$this->_script);
		$this->block('style',	$this->_style);
		$this->block('meta',	$this->_meta);
		return $this->render();
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER PAGE HEADER
	////////////////////////////////////////////////////////////////////////////
	public function headerPage() {
		global $site;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()  &&  file_exists($root.'/header_page_debug.tpl.'.$device)) {
			$this->load($root.'/header_page_debug.tpl.'.$device);
		} else if ($this->debug()  &&  file_exists($root.'/header_page_debug.tpl')) {
			$this->load($root.'/header_page_debug.tpl');
		} else {
			$this->load($root.'/header_page.tpl');
		}

		return $this->merge($this->_headers)->render();
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER HTML FOOTER
	////////////////////////////////////////////////////////////////////////////
	public function footerHTML() {
		global $site;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()) {
			if (file_exists($root.'/footer_html_debug.tpl.'.$device)) {
				return $this->load($root.'/footer_html_debug.tpl.'.$device);
			}
			if (file_exists($root.'/footer_html_debug.tpl')) {
				return $this->render($root.'/footer_html_debug.tpl');
			}
		}

		return $this->render($root.'/footer_html.tpl');
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER PAGE FOOTER
	////////////////////////////////////////////////////////////////////////////
	public function footerPage() {
		global $site, $db;

		$device	= afdevice::device();
		$root	= $this->path() . $site['root'];

		if ($this->debug()  &&  file_exists($root.'/footer_page_debug.tpl.'.$device)) {
			$this->load($root.'/footer_page_debug.tpl.'.$device);
		} else if ($this->debug()  &&  file_exists($root.'/footer_page_debug.tpl')) {
			$this->load($root.'/footer_page_debug.tpl');
		} else {
			$this->load($root.'/footer_page.tpl');
		}

		$this->merge($this->_footers)->render();

		if ($this->hasPermission(['staff','admin'])  &&  $this->debug()  &&  file_exists($root.'/footer_admin.tpl')) {
			$this->load($root.'/footer_admin.tpl');
			if (!empty($db)) {
				$this->field('dbstats',		$db->stats());
				$this->block('dbmisses',	$db->stats()['missed']);
			}
			$this->render();
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	//RENDER AN ENTIRE PAGE
	////////////////////////////////////////////////////////////////////////////
	public function renderPage($file, $merge=false) {
		global $title, $og;

		//Allow device specific loading
		$device	= $file . '.' . afdevice::device();

		//Pull the contents of the template BEFORE anything else!
		$text = @file_get_contents( file_exists($device) ? $device : $file );
		if ($text === false) {
			trigger_error('Unable to load template file: '.$file, E_USER_ERROR);
		}

		//PULL OUT REQUIRED PERMISSIONS TAG
		preg_match("/\<permission[^>]*>(.*)\<\/permission\>/", $text, $matches);
		if (!empty($matches[1])) {
			if ($matches[1] === 'login') {
				$this->requireLogin();
			} else {
				$this->requirePermission($matches[1]);
			}
			$text = preg_replace("/\<permission[^>]*>(.*)\<\/permission\>/", '', $text);
		}

		//PULL OUT TITLE TAG
		preg_match("/\<title[^>]*>(.*)\<\/title\>/", $text, $matches);
		if (!empty($matches[1])) {
			$title	= $this->renderString($matches[1]);
			$text	= preg_replace("/\<title[^>]*>(.*)\<\/title\>/", '', $text);
		}

		//PULL OUT STYLE TAG
		preg_match("/\<style[^>]*>(.*)\<\/style\>/s", $text, $matches);
		if (!empty($matches[1])) {
			$this->sheet($this->renderString($matches[1]));
			$text	= preg_replace("/\<style[^>]*>(.*)\<\/style\>/s", '', $text);
		}

		//PULL OUT DESCRIPTION TAG
		preg_match("/\<description[^>]*>(.*)\<\/description\>/s", $text, $matches);
		if (!empty($matches[1])) {
			$og['description'] = $this->renderString($matches[1]);
			$text	= preg_replace("/\<description[^>]*>(.*)\<\/description\>/s", '', $text);
		}

		//PULL OUT IMAGE TAG
		preg_match("/\<image[^>]*>(.*)\<\/image\>/", $text, $matches);
		if (!empty($matches[1])) {
			$og['image'] = $this->renderString($matches[1]);
			$text	= preg_replace("/\<image[^>]*>(.*)\<\/image\>/", '', $text);

			if (!empty($og['twittername'])  &&  !empty($og['twitterdomain'])) {
				$this->metas([
					['name'=>'twitter:card',		'content'=>'photo'],
					['name'=>'twitter:site',		'content'=>$og['twittername']],
					['name'=>'twitter:domain',		'content'=>$og['twitterdomain']],
					['name'=>'twitter:title',		'content'=>$title],
					['name'=>'twitter:image',		'content'=>$og['image']],
					['name'=>'twitter:description',	'content'=>$og['description']],
				]);
			}
		}

		//Render all the things!
		return $this
			->header()
				->loadString($text)
					->merge($merge)
				->render()
			->footer();
	}




	private $_script	= [];
	private $_style		= [];
	private $_sheet		= [];
	private $_meta		= [];
	private $_headers	= [];
	private $_footers	= [];

}