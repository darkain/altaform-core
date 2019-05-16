<?php


class afSidebar {


	public function __construct($data=true) {
		if (tbx_array($data)) $this->data = $data;
		if ($data === true) $this->fetch()->process();
	}



	public function __toString() {
		return $this->renderString();
	}



	public function fetch($table='menu', $clause=false) {
		global $db;

		$this->data = [];
		$data = $db->rows($table, $clause, ['menu_sort', 'menu_id']);

		if (pudl_array($data)) {
			foreach ($data as $item) {
				$this->data[reset($item)] = $item;
			}
		}

		return $this;
	}



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



	public function renderString($menu=false) {
		global $af;

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
				$text .= $this->renderString($item['children']) . '</div>';

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



	public function render() {
		echo $this->renderString();
		return $this;
	}



	public function complete() {
		if (empty($this->data)) $this->fetch();
		return $this->renderString();
	}



	public function base($base) {
		$this->base = $base;
		return $this;
	}



	private	$data		= [];
	private	$menu		= [];
	private $base		= '';

	public	$template	= [
		'break'			=> '<hr/>',
		'parent'		=> '<span><i>&#9658;</i> [menu.menu_text]</span>',

		'child'			=>
			'<a href="[afurl.base][base][menu.menu_url]" target="[menu.menu_target;magnet=#]">' .
				'[menu.menu_text] ' .
				'<em class="small">[menu.menu_subtext;noerr;magnet=em]</em>' .
			'</a>',
	];
}

