import { useEffect, useRef } from "react";

function useWatch(value: any, callback: (value: any, prevValue: any) => void) {
  const prevValue = useRef(value);

  useEffect(() => {
    if (prevValue.current !== value) {
      callback(value, prevValue.current);
      prevValue.current = value;
    }
  }, [value, callback]);
}

export default useWatch;
