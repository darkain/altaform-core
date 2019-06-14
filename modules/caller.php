<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// POLYFILL TO ENABLE CALLABLE VARIABLE METHODS
////////////////////////////////////////////////////////////////////////////////
trait caller {




	////////////////////////////////////////////////////////////////////////////
	// THE MAGICAL __CALL METHOD THAT DOES THE TRANSLATING
	////////////////////////////////////////////////////////////////////////////
	public function __call($function, $arguements) {
		if (isset($this->$function)  &&  is_callable($this->$function)) {
			return call_user_func_array($this->$function, $arguements);
		}

		throw new exception\method('Function not found: ' . $function);
	}

}
