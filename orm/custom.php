<?php



require_once('orm.php');



class			afo_custom
	extends		afo_orm {
	use			afo_template;




	////////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR
	////////////////////////////////////////////////////////////////////////////
	public function __construct(pudl $pudl, $file, $item=false, $fetch=false) {
		parent::__construct($pudl, $item, $fetch);
		$this->file = $file;
	}




	////////////////////////////////////////////////////////////////////////////
	// URL TO THIS OBJECT
	////////////////////////////////////////////////////////////////////////////
	public function url() {}




	////////////////////////////////////////////////////////////////////////////
	// RENDER
	////////////////////////////////////////////////////////////////////////////
	public function render($af) {
		$af	->load($this->file)
			->field('item', $this)
			->render();
	}




	////////////////////////////////////////////////////////////////////////////
	// LOCAL VARIABLES
	////////////////////////////////////////////////////////////////////////////
	private $file = '';




	////////////////////////////////////////////////////////////////////////////
	// LATE STATIC BINDING VARIABLES FROM PUDL ORM
	////////////////////////////////////////////////////////////////////////////
	const	column		= '';
	const	table		= '';

}
