<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// A SIMPLE CLASS FOR GENERATING A MULTI-LEVEL MENU SIDEBAR
////////////////////////////////////////////////////////////////////////////////
class sidebar {


	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(\pudl $pudl=NULL, $data=true) {
		$this->pudl = $pudl;

		// PROCESS DATA
		if (tbx_array($data)) {
			$this->data = $data;

		} else if ($data === true) {
			$this->fetch()->process();
		}
	}




	////////////////////////////////////////////////////////////////////////////
	// RENDER MENU TO STRING USING GLOBAL AF PROCESSOR
	////////////////////////////////////////////////////////////////////////////
	public function __toString() {
		global $af;
		return $this->renderString($af);
	}




	////////////////////////////////////////////////////////////////////////////
	// FETCH MENU DATA FROM TABLE IN DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function fetch($table='menu', $clause=NULL) {
		$this->data = [];

		// EARLY OUT
		if (!($this->pudl instanceof \pudl)) return $this;

		// PULL DATA FROM TABLE IN DATABASE
		$data = $this->pudl->rows($table, $clause, ['menu_sort', 'menu_id']);

		if (pudl_array($data)) {
			foreach ($data as $item) {
				$this->data[reset($item)] = $item;
			}
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// PROCESS FLAT ITEMS INTO A TREE
	////////////////////////////////////////////////////////////////////////////
	public function process() {
		foreach ($this->data as &$item) {
			if (empty($item['menu_parent'])) {
				$this->menu[$item['menu_id']] = &$item;
				continue;
			}

			if (empty($this->data[$item['menu_parent']])) continue;

			$item['parent'] = $this->data[$item['menu_parent']];
			$this->data[$item['menu_parent']]['children'][] = &$item;
		} unset($item);

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// GENERATE MENU STRING AND RETURN IT
	////////////////////////////////////////////////////////////////////////////
	public function renderString(\altaform $af, $menu=false) {
		if ($menu === false) $menu = $this->menu;

		// BACKUP CURRENT STRING, AND PREP OUTPUT TEXT
		$temp = (string) $af;
		$text = '';

		foreach ($menu as $item) {
			if (!empty($item['children'])) {
				$af->loadString( $this->template['parent'] );
				$af->field('menu', $item);
				$text .= '<div data-af-sidebar-menu="' . $item['menu_id'] . '">';
				$text .= $af->renderToString();
				$text .= $this->renderString($af, $item['children']) . '</div>';

			} else if (is_null($item['menu_text'])) {
				$text .= $af->renderString( $this->template['break'] );

			} else {
				$af->loadString( $this->template['child'] );
				$af->field('base', empty($item['menu_target']) ? $this->base : '');
				$af->field('menu', $item);
				$text .= $af->renderToString();
			}
		}

		// RESTORE PREVIOUSLY LOADED STRING
		$af->loadString($temp);

		return $text;
	}




	////////////////////////////////////////////////////////////////////////////
	// RENDER DATA TO CLIENT
	////////////////////////////////////////////////////////////////////////////
	public function render(\altaform $af) {
		echo $this->renderString($af);
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// FETCH DATA AND RENDER IT
	////////////////////////////////////////////////////////////////////////////
	public function complete(\altaform $af) {
		if (empty($this->data)) $this->fetch();
		return $this->renderString($af);
	}




	////////////////////////////////////////////////////////////////////////////
	// SET THE URL BASE
	////////////////////////////////////////////////////////////////////////////
	public function base($base) {
		$this->base = $base;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// MEMBER VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private		$data		= [];
	private		$menu		= [];
	private		$base		= '';

	protected	$pudl		= NULL;

	public		$template	= [
		'break'				=> '<hr/>',
		'parent'			=> '<span><i>&#9658;</i> [menu.menu_text]</span>',

		'child'				=>
			'<a href="[afurl.base][base][menu.menu_url]" target="[menu.menu_target;magnet=#]">' .
				'[menu.menu_text] ' .
				'<em class="small">[menu.menu_subtext;noerr;magnet=em]</em>' .
			'</a>',
	];
}

