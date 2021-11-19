<?php
/** @generate-function-entries */
class Carray implements  IteratorAggregate, ArrayAccess, Countable {
	public function __construct(int $typeEnum, int $size = 0) {}

    public function count() : int {}

    /** @return array */
    public function toArray() {}

    /** @return int */
    public  function getTypeEnum() {}

    /** @return string */
    public function getTypeName() {}
    
    /** @return int */
    public function getTypeSize() {}

    /** @return void */
    public function reserve(int $size);

    /** @return int */
    public function size() {}

     /** @return int */
    public function capacity() {}

    /** @return bool */
    public function resize(int $size) {}

    /** @return bool */
    public function signedType() {}


	public function offsetExists(mixed $index) : bool {}

    public function offsetGet(mixed $index) : mixed {}


    public function offsetSet(mixed $index, mixed $value) : void {}


    public function offsetUnset(mixed $index) : void {}


    public function getIterator(): Iterator {}
     
  
    public function sub(int $start, int $len) : CArray {}
}

/** @generate-function-entries */
class Csu8 implements IteratorAggregate {

    public function __construct(string $s) {}

    public function __toString() : string {}


    public function getIterator(): Iterator {}

}

   /** @generate-function-entries */
class Csu32 extends CArray {
   
    public function __construct(string $s) {}

    public function __toString() : string {}
}

   /** @generate-function-entries */
