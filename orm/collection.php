<?php


require_once('_pudl/pudlCollection.php');




class		afo_collection
	extends	pudlCollection {




	////////////////////////////////////////////////////////////////////////////
	// PROMETHEUS RENDERER
	////////////////////////////////////////////////////////////////////////////
	public function render($af) {
		$class	= $this->classname();
		$list	= $this->raw();
		$last	= false;
		$data	= '';

		foreach ($this->data as $key => $value) {
			$data .= ' data-'.$af->html($key).'="'.$af->html($value).'"';
		}

		if (!empty($this->header)) {
			$af	->load($this->header['template'])
				->merge($this->header)
				->render();
		}

		foreach ($list as $item) {
			if (!($item instanceof afo_orm)) continue;

			$item->prometheus();

			if ($this->label) {
				$item->title = $item->{$this->separate} . ' ' . $this->label;
			} else if ($this->separate) {
				$item->title = $item->{$this->separate};
			}

			if ($last === false) $this->header($af, $this, $item, $data);

			if ($this->separate !== false) {
				if ($last !== false  &&  $last !== $item->{$this->separate}) {
					$this->footer($af, $this);
					$this->header($af, $this, $item, $data);
				}
				$last = $item->{$this->separate};
			} else {
				$last = true;
			}

			$item->render($af);
		}

		if ($last) $this->footer($af, $this);

		if (!empty($this->footer)) {
			$af	->load($this->footer['template'])
				->merge($this->footer)
				->render();
		}

		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// PROMETHEUS RENDERER
	////////////////////////////////////////////////////////////////////////////
	public function separate($field, $label=false) {
		$this->separate	= $field;
		$this->label	= $label;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// ADD HTML DATA PROPERTIES
	////////////////////////////////////////////////////////////////////////////
	public function data($key, $value) {
		$this->data[$key] = $value;
		return $this;
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private		$data		= [];
	protected	$separate	= false;
	protected	$label		= false;
	public		$header		= false;
	public		$footer		= false;
	public		$id			= false;
	public		$class		= false;
}

