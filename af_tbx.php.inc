<?php


trait af_tbx {


	////////////////////////////////////////////////////////////////////////////
	//CUSTOM TEMPLATE FORMATS
	////////////////////////////////////////////////////////////////////////////
	protected function _customFormat(&$text, $style) {
		global $afurl;

		switch ($style) {
			case 'cdn':
				$text = $afurl->cdn($text);
			break;

			case 'url':
				$text = afstring::url($text);
			break;

			case 'linkify':
				$text = afstring::linkify($text);
			break;

			default:
				parent::_customFormat($text, $style);
		}
	}


}