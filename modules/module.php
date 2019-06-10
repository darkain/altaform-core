<?php

namespace af;




////////////////////////////////////////////////////////////////////////////////
// LOAD THE GIVEN ALTAFORM MODULE
////////////////////////////////////////////////////////////////////////////////
function module($__af_name__) {
	static $__af_loaded__ = [];
	if (in_array($__af_name__, $__af_loaded__)) return;
	$__af_loaded__[] = $__af_name__;

	extract($GLOBALS, EXTR_REFS | EXTR_SKIP);

	require(is_owner(implode('/', [
		__DIR__,
		$__af_name__ . '.php'
	])));

	$__af_list__ = get_defined_vars();

	unset($__af_list__['__af_name__']);
	unset($__af_list__['__af_loaded__']);

	foreach ($__af_list__ as $__af_key__ => $__af_value__) {
		$GLOBALS[$__af_key__] = $__af_value__;
	}
}
