<?php


namespace {
	class afException extends Exception {}
}


namespace af\exception {
	class import extends \afException {}
	class config extends \afException {}
	class method extends \afException {}
}
