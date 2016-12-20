<?php


require_once('af_object_array.php.inc');



if (!interface_exists('pudlId')) {
	interface db_object_interface {}
} else {
	interface db_object_interface extends pudlId {}
}




abstract class	db_object
	extends		af_object_array
	implements	db_object_interface {




	public function __construct($table, $column, $item=false, $fetch=false) {
		$this->_table	= $table;
		$this->_column	= $column;

		if (is_array($item)) {
			$fetch ? $this->fetch($item)		: $this->_replace($item);

		} else if (is_a($item, 'afurl')) {
			$this->fetch($item->id);

		} else if (is_a($item, 'getvar')) {
			$this->fetch($item->id());

		} else if (is_a($item, 'pudlResult')) {
			$fetch ? $this->fetch($item())		: $this->_clone($item());

		} else if (is_a($item, 'Traversable')) {
			$fetch ? $this->fetch($item)		: $this->_clone($item);

		} else if ($fetch  &&  (is_int($item)  ||  ctype_digit($item))) {
			$this->fetch($item);

		} else if (!empty($item)) {
			$this->{$column} = $item;
		}


		//SET DEFAULT OBJECT ID AND FORCE INTEGER
		$this->{$column} =	(!empty($this->{$column}))
							? (int) $this->{$column}
							: 0;
	}




	////////////////////////////////////////////////////////////////////////////
	//CREATE A NEW INSTANCE OF THIS OBJECT IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	//abstract public static function create($data=false);




	////////////////////////////////////////////////////////////////////////////
	//GET AN INSTANCE OF THIS OBJECT FROM THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	//abstract public static function get($id);




	////////////////////////////////////////////////////////////////////////////
	//IF CONVERTING CLASS TO STRING, RETURNS THE CURRENT USER ID NUMBER
	////////////////////////////////////////////////////////////////////////////
	public function __toString() {
		return (string) $this->{$this->_column};
	}




	////////////////////////////////////////////////////////////////////////////
	//RETURN THE CURRENT OBJECT ID NUMBER
	////////////////////////////////////////////////////////////////////////////
	public function id() {
		return (int) $this->{$this->_column};
	}




	////////////////////////////////////////////////////////////////////////////
	//COMPARE ITEM TO SEE IF IT IS THE CURRENT OBJECT INSTANCE
	////////////////////////////////////////////////////////////////////////////
	public function is($item) {
		if ($item === true)		return $this->id() > 0;
		if ($item === false)	return $this->id() === 0;
		if (empty($item))		return false;

		if (is_array($item)) {
			if (empty($item[$this->_column])) return false;
			$item = $item[$this->_column];

		} else if (is_object($item)) {
			if (empty($item->{$this->_column})) return false;
			$item = $item->{$this->_column};
		}

		$item = (int) $item;
		if ($item === 0) return false;

		return $item === $this->id();
	}




	////////////////////////////////////////////////////////////////////////////
	//UPDATE THE USER TABLE IN THE DATABASE
	////////////////////////////////////////////////////////////////////////////
	public function update($data) {
		global $db;

		if (tbx_array($data)) {
			foreach ($data as $key => $item) {
				$this->{$key} = $item;
			}
		}

		return $db->updateId($this->_table, $data, $this);
	}




	////////////////////////////////////////////////////////////////////////////
	//PUDL INTEGRATION
	////////////////////////////////////////////////////////////////////////////
	public function pudlId() {
		return [$this->_column => $this->{$this->_column}];
	}




	////////////////////////////////////////////////////////////////////////////
	//FETCH DATA FROM DATABASE
	////////////////////////////////////////////////////////////////////////////
	protected function fetch($id) {
		global $db;

		if (is_array($id)) {
			$id = empty($id[$this->_column])	? 0 : $id[$this->_column];

		} else if (is_object($id)) {
			$id = empty($id->{$this->_column})	? 0 : $id->{$this->_column};
		}

		$this->_clone(
			$db	->cache($this->_fetchCache())
				->rowId($this->_table, $this->_column, (int)$id)
		);
	}




	////////////////////////////////////////////////////////////////////////////
	//HOW LONG SHOULD THE FETCHED DATA BE CACHED FOR (IN SECONDS)
	////////////////////////////////////////////////////////////////////////////
	protected function _fetchCache() { return 0; }




	////////////////////////////////////////////////////////////////////////////
	//SHORTCUT FUNCTIONS FOR ERROR CHECKING
	////////////////////////////////////////////////////////////////////////////
	public function assert401($text=false) { assert401((string)$this, $text); return $this; }
	public function assert404($text=false) { assert404((string)$this, $text); return $this; }




	protected $_column	= '';
	protected $_table	= '';
}
