<?php

class afclosure {
	public function __set($name, $value) {
		if (is_callable($value)) {
			$this->_functions[$name] = $value;
		} else {
			$this->$name = $value;
		}
	}

	public function __call($function, $arguements) {
		if (isset($this->_functions[$function])) {
			return call_user_func_array($this->_functions[$function], $arguements);
		}
	}

	private $_functions	= [];
}
