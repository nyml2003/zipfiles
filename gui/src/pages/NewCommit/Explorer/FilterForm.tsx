import React from 'react';
import { Button, DatePicker, Form, Input, InputNumber, Mentions, Select, Space } from 'antd';
import { FileType } from '@/types';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { updateFilter, updateIsFiltering } from '@/stores/CreateCommitReducer';
import dayjs from 'dayjs';
type Filter = Partial<{
  type: FileType;
  name: string;
  minSize: number;
  maxSize: number;
  minCreateTime: number;
  maxCreateTime: number;
  minUpdateTime: number;
  maxUpdateTime: number;
  owner: string;
  group: string;
}>;
const fileTypeOptions = [
  { label: '文件', value: FileType.Regular },
  { label: '文件夹', value: FileType.Directory },
  { label: '链接', value: FileType.Symlink },
  { label: '块设备', value: FileType.Block },
  { label: '字符设备', value: FileType.Character },
  { label: 'FIFO', value: FileType.Fifo },
  { label: 'Socket', value: FileType.Socket },
];

const FilterForm: React.FC = () => {
  const [form] = Form.useForm();
  const filter = useSelector((state: RootState) => state.createCommit.filter);
  const formFilter = {
    type: filter.type,
    minSize: filter.minSize,
    maxSize: filter.maxSize,
    minCreateTime: filter.minCreateTime
      ? dayjs(filter.minCreateTime * 1000)
      : undefined,
    maxCreateTime: filter.maxCreateTime
      ? dayjs(filter.maxCreateTime * 1000)
      : undefined,
    minUpdateTime: filter.minUpdateTime
      ? dayjs(filter.minUpdateTime * 1000)
      : undefined,
    maxUpdateTime: filter.maxUpdateTime
      ? dayjs(filter.maxUpdateTime * 1000)
      : undefined,
    owner: filter.owner,
    group: filter.group,
  };
  const dispatch = useDispatch();

  const onReset = () => {
    form.resetFields();
    dispatch(updateFilter({}));
    dispatch(updateIsFiltering(false));
  };

  const onFinish = (values: Filter) => {
    const filter: Filter = {};
    filter.group = values.group || undefined;
    filter.maxSize = values.maxSize || undefined;
    filter.minSize = values.minSize || undefined;
    filter.owner = values.owner || undefined;
    filter.type = values.type || undefined;
    filter.maxCreateTime = values.maxCreateTime ? values.maxCreateTime.valueOf() / 1000 : undefined;
    filter.minCreateTime = values.minCreateTime ? values.minCreateTime.valueOf() / 1000 : undefined;
    filter.maxUpdateTime = values.maxUpdateTime ? values.maxUpdateTime.valueOf() / 1000 : undefined;
    filter.minUpdateTime = values.minUpdateTime ? values.minUpdateTime.valueOf() / 1000 : undefined;
    dispatch(updateFilter(filter));
    dispatch(updateIsFiltering(false));
  };

  return (
    <div className='fade-in-down p-4 max-w-3xl'>
      <Form
        labelCol={{ span: 4 }}
        wrapperCol={{ span: 14 }}
        layout='horizontal'
        variant='outlined'
        scrollToFirstError
        form={form}
        initialValues={formFilter}
        onFinish={onFinish}>
        <Form.Item
          label='文件类型'
          name='type'
          rules={[{ required: false, message: 'Please select file type!' }]}>
          <Select options={fileTypeOptions} />
        </Form.Item>
        <Form.Item label='大小'>
          <Space.Compact>
            <Form.Item
              name={['minSize']}
              noStyle
              rules={[
                {
                  type: 'number',
                  validator: (rule, value) => {
                    if (value === undefined) {
                      return Promise.resolve();
                    }
                    if (
                      form.getFieldValue(['maxSize']) !== undefined &&
                      value > form.getFieldValue(['maxSize'])
                    ) {
                      return Promise.reject('最小值不能大于最大值');
                    }
                    return Promise.resolve();
                  },
                },
              ]}>
              <InputNumber style={{ width: '40%' }} placeholder='最小值' min={0} />
            </Form.Item>
            <Input
              style={{
                width: '20%',
                textAlign: 'center',
              }}
              placeholder='~'
              disabled
            />
            <Form.Item
              name={['maxSize']}
              noStyle
              rules={[
                {
                  type: 'number',
                  validator: (rule, value) => {
                    if (value === undefined) {
                      return Promise.resolve();
                    }
                    if (
                      form.getFieldValue(['minSize']) !== undefined &&
                      value < form.getFieldValue(['minSize'])
                    ) {
                      return Promise.reject('最大值不能小于最小值');
                    }
                    return Promise.resolve();
                  },
                },
              ]}>
              <InputNumber style={{ width: '40%' }} placeholder='最大值' min={0} />
            </Form.Item>
          </Space.Compact>
        </Form.Item>
        <Form.Item label='创建时间'>
          <Space.Compact>
            <Form.Item
              noStyle
              name='minCreateTime'
              rules={[
                {
                  type: 'number',
                  validator: (rule, value) => {
                    if (value === undefined) {
                      return Promise.resolve();
                    }
                    if (
                      form.getFieldValue(['maxCreateTime']) !== undefined &&
                      value > form.getFieldValue(['maxCreateTime'])
                    ) {
                      return Promise.reject('时间区间不合法');
                    }
                    return Promise.resolve();
                  },
                },
              ]}>
              <DatePicker
                showTime
                style={{ width: '40%' }}
                placeholder='最小值'
              />
            </Form.Item>
            <Input
              style={{
                width: '20%',
                textAlign: 'center',
              }}
              placeholder='~'
              disabled
            />
            <Form.Item
              name='maxCreateTime'
              noStyle
              rules={[
                {
                  type: 'number',
                  validator: (rule, value) => {
                    if (value === undefined) {
                      return Promise.resolve();
                    }
                    if (
                      form.getFieldValue(['minCreateTime']) !== undefined &&
                      value < form.getFieldValue(['minCreateTime'])
                    ) {
                      return Promise.reject('时间区间不合法');
                    }
                    return Promise.resolve();
                  },
                },
              ]}>
              <DatePicker
                showTime
                style={{ width: '40%' }}
                placeholder='最大值'
              />
            </Form.Item>
          </Space.Compact>
        </Form.Item>

        <Form.Item label='更新时间'>
          <Space.Compact>
            <Form.Item
              noStyle
              name='minUpdateTime'
              rules={[
                {
                  type: 'number',
                  validator: (rule, value) => {
                    if (value === undefined) {
                      return Promise.resolve();
                    }
                    if (
                      form.getFieldValue(['maxUpdateTime']) !== undefined &&
                      value > form.getFieldValue(['maxUpdateTime'])
                    ) {
                      return Promise.reject('时间区间不合法');
                    }
                    return Promise.resolve();
                  },
                },
              ]}>
              <DatePicker
                showTime
                style={{ width: '40%' }}
                placeholder='最小值'
              />
            </Form.Item>
            <Input
              style={{
                width: '20%',
                textAlign: 'center',
              }}
              placeholder='~'
              disabled
            />
            <Form.Item
              name='maxUpdateTime'
              noStyle
              rules={[
                {
                  type: 'number',
                  validator: (rule, value) => {
                    if (value === undefined) {
                      return Promise.resolve();
                    }
                    if (
                      form.getFieldValue(['minUpdateTime']) !== undefined &&
                      value < form.getFieldValue(['minUpdateTime'])
                    ) {
                      return Promise.reject('时间区间不合法');
                    }
                    return Promise.resolve();
                  },
                },
              ]}>
              <DatePicker
                showTime
                style={{ width: '40%' }}
                placeholder='最大值'
              />
            </Form.Item>
          </Space.Compact>
        </Form.Item>

        <Form.Item
          label='所有者'
          name='owner'
          rules={[{ required: false, message: 'Please input owner!' }]}>
          <Mentions rows={1} placeholder='输入用户名' />
        </Form.Item>

        <Form.Item
          label='组'
          name='group'
          rules={[{ required: false, message: 'Please input group!' }]}>
          <Mentions rows={1} placeholder='输入组名' />
        </Form.Item>

        <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
          <Space>
            <Button type='primary' htmlType='submit'>
              Submit
            </Button>
            <Button htmlType='button' onClick={onReset}>
              Reset
            </Button>
          </Space>
        </Form.Item>
      </Form>
    </div>
  );
};

export default FilterForm;
