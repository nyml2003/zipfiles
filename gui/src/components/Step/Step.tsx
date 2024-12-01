import React, { ReactNode } from "react";
export interface StepProps {
  status: "pending" | "running" | "completed" | "failed" | "cancel";
  title: string;
  description?: ReactNode;
}
import styles from "./Step.module.less";

export const Step = ({ status, title, description }: StepProps) => {
  const getStatusIcon = (status: string) => {
    const iconClasses = "w-6 h-6 flex items-center justify-center rounded-full text-white text-xs";
    switch (status) {
      case "completed":
        return <div className={`${iconClasses} bg-green-500 text-white`}>✓</div>;
      case "failed":
        return <div className={`${iconClasses} bg-red-500 text-white`}>✕</div>;
      case "running":
        return <div className={`${iconClasses} bg-blue-500 ${styles.spinner}`}>⏳</div>;
      case "pending":
        return <div className={`${iconClasses} bg-gray-500`}>···</div>;
      case "cancel":
        return <div className={`${iconClasses} bg-yellow-500`}>⚠️</div>;
      default:
        return null;
    }
  };

  return (
    <div className='flex p-2 flex-col'>
      <div className='flex flex-row items-center'>
        {getStatusIcon(status)}
        <h3 className='ml-4 text-lg font-semibold truncate'>{title}</h3>
      </div>
      {description && (
        <div className='mt-2 border-2 border-dashed rounded-lg p-2 text-gray-500'>{description}</div>
      )}
    </div>
  );
};
