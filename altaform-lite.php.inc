<?php

//Tiny But Strong Template Library (TBS)
require_once('_tbs/tbs_class.php');


class altaformlite extends clsTinyButStrong {
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
	}


	public function render($filename=false) {
		if ($filename !== false) $this->load($filename);
		$this->Show();
	}


	public function renderBlock($filename, $block, $data) {
		$this->load($filename);
		$this->block($block, $data);
		$this->Show();
	}


	public function renderField($filename, $field, $data) {
		$this->load($filename);
		$this->field($field, $data);
		$this->Show();
	}


	public function loadString($template) {
		$this->Source = $template;
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

	//Shorthand name for MergeField from TBS
	public function field($NameLst, $Value='assigned', $IsUserFct=false, $DefaultPrm=false) {
		return parent::MergeField($NameLst, $Value, $IsUserFct, $DefaultPrm);
	}

	//Shorthand name for MergeBlock from TBS
	public function block($BlockLst, $SrcId='assigned', $Query='', $QryPrms=false) {
		return parent::MergeBlock($BlockLst, $SrcId, $Query, $QryPrms);
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
