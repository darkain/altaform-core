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

			case 'hex':
				$text = bin2hex($text);
			break;

			//TODO: this is deprecated, instead date=Y/M,f=lower (to support ALL date formats)
			case 'urldate':
				$text = strtolower(date('Y/M', $text));
			break;

			case 'linkify':
				$text = afstring::linkify($text);
			break;

			default:
				parent::_customFormat($text, $style);
		}
	}




	////////////////////////////////////////////////////////////////////////////
	//LOG TBX ERROR MESSAGES
	////////////////////////////////////////////////////////////////////////////
	function meth_Misc_Alert($Src, $Msg, $NoErrMsg=false, $SrcType=false, $data='') {
		ob_start();
		$ret = parent::meth_Misc_Alert($Src, $Msg, $NoErrMsg, $SrcType);
		$text = trim(ob_get_clean());

		_af_log_error([
			'file'		=> $this->filepath,
			'details'	=> $text,
			'tbx'		=> json_encode($Src),
			'msg'		=> json_encode($Msg),
			'data'		=> json_encode($data),
		], $this->debug());

		echo $text;
		return $ret;
	}


}