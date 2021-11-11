<?php

class utoken {

	public int $start;
	public int $end;
	public string $value;
	public int    $code; // values at the start of the match;

	public int    $expid; // match regular expression token id
    public int    $line; // line number in stream;
    public bool   $isSingle; // if single character match
}