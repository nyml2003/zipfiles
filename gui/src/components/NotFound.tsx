import React from "react";
import Button from "./Button";

const NotFoundLayout = () => {
  return (
    <div className='flex flex-col items-center justify-center h-full'>
      <div className='text-4xl font-bold'>404</div>
      <p className='text-2xl'>页面不存在</p>
      <Button
        className='mt-4 px-4 py-2 bg-blue-500 text-white rounded'
        onClick={() => {
          window.location.href = "/";
        }}>
        返回首页
      </Button>
    </div>
  );
};

export default NotFoundLayout;
