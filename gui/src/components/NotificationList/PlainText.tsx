import { PlainText as PlainTextProps } from "./types";
import React from "react";
import {
  CheckCircleOutlined,
  InfoCircleOutlined,
  ExclamationCircleOutlined,
  CloseCircleOutlined
} from "@ant-design/icons";
const PlainText = ({ text, state, description, id }: PlainTextProps) => {
  const renderIcon = (type: string) => {
    switch (type) {
      case "success":
        return <CheckCircleOutlined className='text-green-500 text-lg ' />;
      case "info":
        return <InfoCircleOutlined className='text-blue-500 text-lg' />;
      case "warning":
        return <ExclamationCircleOutlined className='text-yellow-500 text-lg' />;
      case "error":
        return <CloseCircleOutlined className='text-red-500 text-lg' />;
      default:
        return null;
    }
  };
  return (
    <div className='flex flex-row'>
      <div className='px-2 pt-1'>{renderIcon(state)}</div>
      <div className='grow-item flex flex-col items-start '>
        <div className='text-lg overflow-hidden overflow-ellipsis whitespace-nowrap w-full'>
          {text}
        </div>
        {description && <div className='text-gray-500 text-sm mt-2 mr-4'>{description}</div>}
      </div>
    </div>
  );
};

export default PlainText;
