export function createDilute(interval: number) {
  const queue: (() => void)[] = [];
  let intervalId: NodeJS.Timeout | undefined = undefined;
  let lastExecutionTime = 0;

  return function dilute(func: () => void) {
    queue.push(func);
    if (!intervalId) {
      intervalId = setInterval(() => {
        if (queue.length > 0) {
          if (Date.now() - lastExecutionTime >= interval) {
            const funcToExecute = queue.shift();
            funcToExecute?.();
            lastExecutionTime = Date.now();
          }
        } else {
          clearInterval(intervalId);
          intervalId = undefined;
        }
      }, interval);
    }
  };
}

const useDilute = () => {
  const dilute = createDilute(40);
  return {
    dilute,
  };
};
export default useDilute;
