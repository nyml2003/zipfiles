import React from 'react';

const TestPage = () => {
  return (
    <div>
      <h1>Test Page</h1>
      <div
        className='text-blue-500 cursor-pointer bg-white p-2 rounded'
        onClick={() => {
          console.log(process.env);
        }}>
        Click me to throw an error
      </div>
    </div>
  );
};

export default TestPage;
