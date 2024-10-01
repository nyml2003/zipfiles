interface IMQ<T extends () => void> {
  isProcessing: boolean;
  tasks: T[];
  processQueue: () => void;
}

class MQ<T extends () => void> implements IMQ<T> {
  isProcessing: boolean;
  tasks: T[];
  constructor() {
    this.isProcessing = false;
    this.tasks = [];
  }

  async processQueue() {
    if (this.isProcessing || this.tasks.length === 0) return;
    this.isProcessing = true;

    while (this.tasks.length > 0) {
      const task = this.tasks.shift();
      if (!task) break;

      try {
        await new Promise<void>((resolve, reject) => {
          try {
            task();
            resolve();
          } catch (error) {
            console.log('Task execution failed:', error);
            reject(error);
          }
        });
      } catch (error) {
        console.error('Task execution failed:', error);
      }
    }

    this.isProcessing = false;
  }

  push(task: T) {
    this.tasks.push(task);
    this.processQueue();
  }
}

export default MQ;
