import { ApiEnum } from '@/apis';
import {
  MockManyNotificationsRequest,
  MockManyNotificationsResponse,
} from '@/apis/MockManyNotifications';
import useApi from '@useApi';
import { decrement, increment } from '@/stores/counter/action';
import { RootState } from '@/stores/store';
import { Button, message } from 'antd';
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
  const api = useApi();
  const tryMockManyNotifications = () => {
    api
      .request<MockManyNotificationsRequest, MockManyNotificationsResponse>(
        ApiEnum.MOCK_MANY_NOTIFICATIONS,
        {},
      )
      .then(res => {
        message.success({
          content: JSON.stringify(res),
          duration: 2,
        });
      });
  };
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
      <div>
        <Button onClick={tryMockManyNotifications} type='primary' className='mt-4'>
          测试按钮
        </Button>
      </div>
    </div>
  );
};

export default TestPage;
