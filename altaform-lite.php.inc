<?php

//Tiny But Strong Template Library (TBS)
require_once('_tbx/tbx_class.php');


class altaformlite extends clsTinyButXtreme {
	public function __construct() {
		parent::__construct();

		parent::SetOption('render', TBS_OUTPUT);
		parent::ResetVarRef(true);

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
		$this->meth_Merge_AutoVar($this->Source, true);
		if ($this->OnLoad) $this->meth_Merge_AutoOn($this->Source, 'onload', true, true);
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

	//Custom merger - both field and block supported!
	public function merge($data) {
		foreach ($data as $key => &$val) {
			if (is_object($val)) {
				trigger_error('altaform::merge does not support type Object for key: ' . $key);
			} else if (is_string($val)  ||  is_int($val)  ||  is_float($val)) {
				$this->field($key, $val);
			} else if ($val === false) {
				$this->field($key, []);
			} else if (isset($val[0])  ||  empty($val)) {
				$this->block($key, $val);
			} else {
				$this->field($key, $val);
			}
		} unset($val);
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
