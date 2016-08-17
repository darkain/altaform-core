<?php


class af_object_array implements ArrayAccess, Iterator {

	public function __construct(&$array=NULL, $clone=false) {
		if (!is_array($array)) return;
		$clone ? $this->_clone($array) : $this->_replace($array);
	}


	public function _replace(&$array) {
		if (is_a($array, 'ArrayAccess')) return $this->_clone($array);
		$this->_object_array = &$array;
	}


	public function _clone($array) {
		$this->_object_array[] = [];
		if (!is_array($array)  ||  empty($array)) return;
		foreach($array as $key => $value) {
			$this->_object_array[$key]	= $value;
		}
	}


	public function _merge($array) {
		if (empty($array)) return;
		foreach($array as $key => $value) {
			$this->_object_array[$key]	= $value;
		}
	}


	public function _get() {
		return $this->_object_array;
	}


	public function __set($key, $value) {
		$this->_object_array[$key]		= $value;
	}


	public function offsetSet($key, $value) {
		if (is_null($key)) {
			$this->_object_array[]		= $value;
		} else {
			$this->_object_array[$key]	= $value;
		}
	}


	public function &__get($key) {
		return $this->_object_array[$key];
	}


	public function offsetGet($key) {
		return $this->_object_array[$key];
	}


	public function __isset($key) {
		return isset($this->_object_array[$key]);
	}


	public function offsetExists($key, $isset=true) {
		return $isset
			? isset($this->_object_array[$key])
			: array_key_exists($key, $this->_object_array);
	}


	public function __unset($key) {
		unset($this->_object_array[$key]);
	}


	public function offsetUnset($key) {
		unset($this->_object_array[$key]);
	}


	public function rewind() {
		reset($this->_object_array);
	}


	public function current() {
		return current($this->_object_array);
	}


	public function key() {
		return key($this->_object_array);
	}


	public function next() {
		return next($this->_object_array);
	}


	public function valid() {
		$key = key($this->_object_array);
		return ($key !== NULL && $key !== FALSE);
	}


	private $_object_array = [];

}
