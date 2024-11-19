import React, { ReactNode } from "react";
import { useCallback, useState } from "react";

interface StepProps {
  status: "pending" | "process" | "completed" | "failed" | "cancel";
  title: string;
  description: ReactNode;
}

export const Step = ({ status, title, description }: StepProps) => {
  const getStatusClass = (status: string) => {
    switch (status) {
      case "pending":
        return "bg-gray-200";
      case "process":
        return "bg-blue-200";
      case "completed":
        return "bg-green-200";
      case "failed":
        return "bg-red-200";
      case "cancel":
        return "bg-yellow-200";
      default:
        return "bg-gray-200";
    }
  };

  const getStatusIcon = (status: string) => {
    switch (status) {
      case "completed":
        return <div className='w-6 h-6 bg-green-500 rounded-full'></div>;
      case "failed":
        return <div className='w-6 h-6 bg-red-500 rounded-full'></div>;
      case "process":
        return <div className='w-6 h-6 bg-blue-500 rounded-full'></div>;
      case "pending":
        return <div className='w-6 h-6 bg-gray-500 rounded-full'></div>;
      case "cancel":
        return <div className='w-6 h-6 bg-yellow-500 rounded-full'></div>;
      default:
        return null;
    }
  };

  return (
    <div className={`flex items-center ${getStatusClass(status)} p-4 rounded-lg`}>
      <div className='w-10 h-10 flex items-center justify-center rounded-full mr-4'>
        {getStatusIcon(status)} {/* 使用 getStatusIcon 函数来获取状态图标 */}
      </div>
      <div>
        <h3 className='text-lg font-semibold'>{title}</h3>
        <p className='text-gray-600'>{description}</p>
      </div>
    </div>
  );
};

export interface Status {
  content: ReactNode;
  onEnter?: (status?: Status) => Promise<void>;
  onExit?: () => Promise<void>;
}

interface StepMetaData {
  title: string;
  pending: Status;
  process: Status;
  completed: Status;
  fail: Status;
  cancel: Status;
}

interface ExtendStepMetaData extends StepMetaData {
  status: "pending" | "process" | "completed" | "failed" | "cancel";
  description: ReactNode;
}

interface WorkflowProps {
  steps: StepMetaData[];
}

export const Workflow = (props: WorkflowProps) => {
  const [steps, setSteps] = useState<ExtendStepMetaData[]>(
    props.steps.map(step => ({
      ...step,
      status: "pending",
      description: step.pending.content,
    })),
  );

  const [current, setCurrent] = useState(0);
  const [couldNext, setCouldNext] = useState(true);

  const next = useCallback(async () => {
    setCouldNext(false);
    const nextSteps = [...steps];
    switch (steps[current].status) {
      case "pending":
        await steps[current].pending.onExit?.();
        nextSteps[current].status = "process";
        await nextSteps[current].process.onEnter?.(nextSteps[current].process);
        nextSteps[current].description = nextSteps[current].process.content;
        break;
      case "process":
        await steps[current].process.onExit?.();
        nextSteps[current].status = "completed";
        await nextSteps[current].completed.onEnter?.(nextSteps[current].completed);
        nextSteps[current].description = nextSteps[current].completed.content;
        break;
      case "completed":
        await steps[current].completed.onExit?.();
        nextSteps[current + 1].status = "process";
        await nextSteps[current + 1].process.onEnter?.(nextSteps[current + 1].process);
        nextSteps[current + 1].description = nextSteps[current + 1].process.content;
        setCurrent(prev => prev + 1);
        break;
      default:
        break;
    }
    setSteps(nextSteps);
    setCouldNext(true);
  }, [current, steps]);

  const cancel = useCallback(() => {
    setSteps(prev => {
      const next = [...prev];
      next[current].status = "cancel";
      next[current].cancel.onEnter?.(next[current].cancel);
      next[current].description = next[current].cancel.content;
      return next;
    });
  }, [current]);

  const fail = useCallback(() => {
    setSteps(prev => {
      const next = [...prev];
      next[current].status = "failed";
      next[current].description = next[current].fail.content;
      next[current].fail.onEnter?.(next[current].fail);
      return next;
    });
  }, [current]);

  return (
    <div className='flex flex-col items-center justify-center p-8 space-y-4'>
      <h1 className='text-2xl font-semibold'>Workflow</h1>
      {steps.map((step, index) => (
        <Step key={index} status={step.status} title={step.title} description={step.description} />
      ))}
      <div className='flex space-x-2'>
        <button
          onClick={() => next()}
          disabled={!couldNext}
          className='bg-blue-500 text-white px-4 py-2 rounded hover:bg-blue-600'>
          Next
        </button>
        <button
          onClick={() => cancel()}
          className='bg-yellow-500 text-white px-4 py-2 rounded hover:bg-yellow-600'>
          Cancel
        </button>
        <button
          onClick={() => fail()}
          className='bg-red-500 text-white px-4 py-2 rounded hover:bg-red-600'>
          Fail
        </button>
      </div>
    </div>
  );
};
