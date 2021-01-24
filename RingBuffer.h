class RingBuffer {
  private:

    unsigned buffSize;//populated by constructor parameter
    boolean full = false;
    unsigned readIndex = 0;//Always points to the next element to be read
    unsigned lastIndex; //buffSize -1
    unsigned writeIndex = 0;//Always points to the next element to be written
    unsigned _array[200];//KLUDGE to allocate memory - maximum for buffSize

    void syncIndex() {
      if ( writeIndex == 0 ) readIndex = lastIndex;
      else readIndex = writeIndex - 1;
    }

  public:

    //Add an item to the end of the buffer
    void add(unsigned  val) {
      _array[writeIndex] = val;
      if (writeIndex == lastIndex) {
        writeIndex = 0;
        full = true;
      }
      else writeIndex++;
    }

    //Get the previous item
    unsigned getPrev() {
      unsigned temp = _array[readIndex];
      if (readIndex == 0) readIndex = lastIndex;
      else readIndex--;
      return (temp);
    }

    //Get the next unread item
    unsigned getNext() {
      unsigned temp = _array[readIndex++];
      if (readIndex == buffSize) readIndex = 0;
      return (temp);
    }


    //Returns the arithmetic mean value of the most recent n values
    uint32_t average(unsigned n) {
      uint32_t sum;
      unsigned ctr;
      sum = 0;
      syncIndex();
      for (ctr = n; ctr > 0; ctr--) sum += getPrev();
      return (sum / n);
    }


    //Reinitialize  the buffer
    void clean() {
      for (int i = 0; i < buffSize; i++ ) _array[i] = 0;
      writeIndex = readIndex = 0;
      full = false;
    }

    //Constructor
    RingBuffer(unsigned _size) {
      buffSize = _size;
      lastIndex = buffSize - 1;
      for (int i = 0; i < buffSize; i++ ) _array[i] = 0;
    }
};
