<?php

//Tiny But Strong Template Library (TBS)
require_once('_tbx/tbx_class.php');


class altaformlite extends clsTinyButXtreme {
	public function __construct() {
		parent::__construct();

		parent::SetOption('render', TBS_OUTPUT);

		self::$_time = time();

		self::$_path = getcwd();
		if (substr(self::$_path, -1) !== '/') self::$_path .= '/';
	}



	public function load($filename) {
		$this->LoadTemplate($filename);
		return $this;
	}


	public function render($filename=false) {
		if ($filename !== false) $this->load($filename);
		$this->Show();
		return $this;
	}


	public function renderBlock($filename, $block, $data) {
		$this->load($filename);
		$this->block($block, $data);
		$this->Show();
		return $this;
	}


	public function renderField($filename, $field, $data) {
		$this->load($filename);
		$this->field($field, $data);
		$this->Show();
		return $this;
	}


	public function loadString($template) {
		$this->Source = $template;
		$this->_mergeAuto($this->Source);
		if ($this->OnLoad) $this->_mergeOn($this->Source, 'onload');
		return $this;
	}


	public function renderToString() {
		$tmp = $this->Render;
		$this->Render = TBS_NOTHING;
		$this->Show();
		$this->Render = $tmp;
		return $this->Source;
	}


	public function renderString($template) {
		$this->loadString($template);
		return $this->renderToString();
	}


	public function fileToString($filename) {
		$this->load($filename);
		return $this->renderToString();
	}


	//Shorthand name for MergeField from TBS
	public function field($NameLst, $Value='assigned', $DefaultPrm=false) {
		parent::MergeField($NameLst, $Value, $DefaultPrm);
		return $this;
	}


	//Shorthand name for MergeBlock from TBS
	public function block($BlockLst, $SrcId='assigned', $Query='', $QryPrms=false) {
		parent::MergeBlock($BlockLst, $SrcId, $Query, $QryPrms);
		return $this;
	}


	//Customer merger - field lists only (associative array)
	public function fields($fields) {
		foreach ($fields as $key => $value) {
			$this->field($key, $value);
		}
		return $this;
	}


	//Custom merger - both field and block supported!
	public function merge($data) {
		foreach ($data as $key => $value) {
			if (is_object($value)) {
				trigger_error('altaform::merge does not support type Object for key: ' . $key);
			} else if (is_string($value)  ||  is_int($value)  ||  is_float($value)) {
				$this->field($key, $value);
			} else if ($value === false) {
				$this->field($key, []);
			} else if (isset($value[0])  ||  empty($value)) {
				$this->block($key, $value);
			} else {
				$this->field($key, $value);
			}
		}
		return $this;
	}

	//Another merger helper function!
	public function mergePage($filename, $data) {
		$this->load($filename);
		$this->merge($data);
		$this->render();
		return $this;
	}


	public function path() {
		return self::$_path;
	}


	public function time() {
		return self::$_time;
	}


	static protected $_time;
	static protected $_path;
}
