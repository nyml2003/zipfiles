import React from 'react';

const NotFoundLayout = () => {
  return (
    <div className='flex flex-col items-center justify-center h-full'>
      <h1 className='text-4xl font-bold'>404</h1>
      <p className='text-2xl'>Page Not Found</p>
      <button
        className='mt-4 px-4 py-2 bg-blue-500 text-white rounded'
        onClick={() => {
          window.location.href = '/';
        }}>
        Go Back
      </button>
    </div>
  );
};

export default NotFoundLayout;
