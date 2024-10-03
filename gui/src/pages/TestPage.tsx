import { decrement, increment } from '@/stores/counter/action';
import { RootState } from '@/stores/store';
import { Button } from 'antd';
import React from 'react';
import { useSelector, useDispatch } from 'react-redux';

const Counter: React.FC = () => {
  const count = useSelector((state: RootState) => state.counter.count);
  const dispatch = useDispatch();

  return (
    <div>
      <h1>计数器: {count}</h1>
      <Button onClick={() => dispatch(increment())}>增加</Button>
      <Button onClick={() => dispatch(decrement())}>减少</Button>
    </div>
  );
};

const TestPage = () => {
  return (
    <div>
      <h1>Test Page</h1>
      <div
        className='text-blue-500 cursor-pointer bg-white p-2 rounded'
        onClick={() => {
          console.log(process.env);
        }}>
        点击查看环境变量
      </div>
      <Counter />
    </div>
  );
};

export default TestPage;
