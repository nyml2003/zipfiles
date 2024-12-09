import React from "react";
import { useOutletContext } from "react-router-dom";

const WelcomePage = () => {
  const outletContext : { openTour: () => void } = useOutletContext();
  const openTour = outletContext && outletContext.openTour || (() => { throw new Error("openTour is not defined"); });
  return (
    <div className='grow-item h-full flex items-center justify-center bg-gray-50' style={{ minHeight: "calc(100vh - 56px)" }}>
      <div className='max-w-7xl mx-auto px-4 sm:px-6 lg:px-8'>
        <div className='text-center'>
          <div className='text-4xl tracking-tight font-extrabold text-gray-900 sm:text-5xl md:text-6xl'>
            <span className='block xl:inline'>欢迎使用</span>
            <span className='block text-indigo-600 xl:inline'>文件备份软件</span>
          </div>
          <p className='mt-3 max-w-md mx-auto text-lg text-gray-500 sm:text-xl md:mt-5'>
            您的数据安全是我们的首要任务。我们提供简单、可靠的文件备份解决方案，确保您的文件始终安全无忧。
          </p>
          <div className='mt-5'>
            <button className='inline-flex items-center px-4 py-2 border border-transparent text-base font-medium rounded-md shadow-sm text-white bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500' onClick={() => openTour()}>
              开始备份
            </button>
          </div>
        </div>
      </div>
    </div>
  );
};

export default WelcomePage;
