import React from 'react';
import { Button, Form, Input, InputNumber, Mentions, Select, Space } from 'antd';
import styles from './index.module.less';
import { FileType, Filter } from '@/types';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { updateFilter } from '@/stores/file/reducer';

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
  const filter = useSelector((state: RootState) => state.file.filter);
  const dispatch = useDispatch();

  const onReset = () => {
    form.resetFields();
    dispatch(updateFilter({}));
  };

  const validateSize = (_: any, value: any) => {
    const { min, max } = value || {};
    if (min !== undefined && max !== undefined && min > max) {
      return Promise.reject(new Error('最小值不能大于最大值'));
    }
    return Promise.resolve();
  };

  return (
    <div className={`${styles['fade-in-down']} p-4 max-w-3xl`}>
      <Form
        labelCol={{ span: 4 }}
        wrapperCol={{ span: 14 }}
        layout='horizontal'
        variant='outlined'
        scrollToFirstError
        form={form}
        onFinish={(values: Filter) => {
          dispatch(updateFilter(values));
        }}>
        <Form.Item
          label='文件类型'
          name='type'
          rules={[{ required: false, message: 'Please select file type!' }]}>
          <Select options={fileTypeOptions} />
        </Form.Item>
        <Form.Item label='文件大小' name='size' rules={[{ validator: validateSize }]}>
          <Input.Group compact>
            <Form.Item name={['size', 'min']} noStyle>
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
            <Form.Item name={['size', 'max']} noStyle>
              <InputNumber style={{ width: '40%' }} placeholder='最大值' min={0} />
            </Form.Item>
          </Input.Group>
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
