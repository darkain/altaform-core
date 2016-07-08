<?php


class af_sidebar {


	public function __construct($data=true) {
		if (is_array($data)) $this->data = $data;
		if ($data === true) $this->fetch()->process();
	}



	public function __toString() {
		return $this->renderString();
	}



	public function fetch($table='pudl_menu', $clause=false) {
		global $db;
		$this->data = $db->indexed($table, $clause, ['menu_sort', 'menu_id']);
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
				$af->field('menu', $item);
				$text .= $af->renderToString();
			}
		}

		return $text;
	}



	public function render() {
		echo $this->renderString();
	}



	public function complete() {
		if (empty($this->data)) $this->fetch();
		return $this->renderString();
	}



	private	$data		= [];
	private	$menu		= [];

	public	$template	= [
		'break'			=> '<hr/>',
		'parent'		=> '<span><i>&#9658;</i> [menu.menu_text]</span>',

		'child'			=>
			'<a href="[var.afurl.base][menu.menu_url]">' .
				'[menu.menu_text] ' .
				'<em class="small">[menu.menu_subtext;magnet=em]</em>' .
			'</a>',
	];
}

