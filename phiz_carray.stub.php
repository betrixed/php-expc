<?php
/** @generate-function-entries */
class Carray implements  IteratorAggregate, ArrayAccess, Countable {
	public function __construct(int $typeEnum, int $size = 0) {}

    /** @return int */
    public function count() {}

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

    /**
     * @param int $index
     * @return bool
     */

	public function offsetExists($index) {}

    /**
     * @param int $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param int $index
     * @return void
     */
    public function offsetSet($index, mixed $value) {}

    /**
     * @param int $index
     * @return void
     */
    public function offsetUnset($index) {}


    public function getIterator(): Iterator {}
     
    /** 
     * @param int $start
     * @param int $len
     * @return CArray
     *      
     */    
    public function sub(int $start, int $len) : CArray {}
}

/** @generate-function-entries */
class Csu8 implements IteratorAggregate {
    public function __construct(string $s) {}

    /** @return string */
    public function __toString() : string {}


    public function getIterator(): Iterator {}
}

   /** @generate-function-entries */
class Csu32 extends CArray {
    public function __construct(string $s) {}

    /** @return string */
    public function __toString() : string {}
}

